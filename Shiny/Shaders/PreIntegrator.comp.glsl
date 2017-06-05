// #version 450 core
// layout(local_size_x = 8, local_size_y = 8) in;
#define M_PI 3.14159265358979323846
#define M_INV_PI (1.0/M_PI)
#define FLOAT_EPSILON 1e-6

// arguments for prefilter
layout (binding = 0, std140) uniform InputBuffer
{
	vec4 inputArg0;
	vec4 inputArg1;
};

const vec2 INPUT_ENVMAP_SIZE;
const vec2 INPUT_ENVMAP_MAX_MIPLEVEL;

// input
layout (binding = 0) uniform samplerCube envmap;
// output
layout (rgba32f, binding = 0) uniform image2D destImage;

// clamp to [0.0, 1.0]
float Saturate(float value)
{
	return clamp(value, 0.0, 1.0);
}

// tell if a==b
bool FloatEqual(float a, float b)
{
	return abs(a - b) > FLOAT_EPSILON;
}

// reverse bits of a float
// use bitfieldReverse in OpenGL 4.0+
float radicalInverse_VdC(uint bits)
{
	bits = (bits << 16u) | (bits >> 16u);
	bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
	bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
	bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
	bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
	return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

// Hammersley sequence
vec2 Hammersley(uint i, uint N)
{
    float ri = float(bitfieldReverse(i)) * 2.3283064365386963e-10;
    return vec2(float(i)/float(N), ri);
}

// wrapper of random samples
vec2 GetSample(uint i, uint total)
{
	return Hammersley(i, total);
}

// schlick approximation of the fresnel equation
// f90 = 1.0 all the time in our Shiny Engine
vec3 F_Schlick(in vec3 f0, in float f90, in float u)
{
	return f0 + (f90 - f0) * pow(1.f - u, 5.f);
}

// height correlated version of smith GGX V
float V_SmithGGXCorrelated(float NdotL, float NdotV, float alphaG)
{
	// Original formulation of G_SmithGGX Correlated
	// lambda_v = (-1 + sqrt(alphaG2 * (1 - NdotL2) / NdotL2 + 1)) * 0.5f;
	// lambda_l = (-1 + sqrt(alphaG2 * (1 - NdotV2) / NdotV2 + 1)) * 0.5f;
	// G_SmithGGXCorrelated = 1 / (1 + lambda_v + lambda_l);
	// V_SmithGGXCorrelated = G_SmithGGXCorrelated / (4.0f * NdotL * NdotV);

	// This is the optimize version
	float alphaG2 = alphaG * alphaG;
	// Caution: the "NdotL *" and "NdotV *" are explicitely inversed , this is not a mistake.
	float Lambda_GGXV = NdotL * sqrt((-NdotV * alphaG2 + NdotV) * NdotV + alphaG2);
	float Lambda_GGXL = NdotV * sqrt((-NdotL * alphaG2 + NdotL) * NdotL + alphaG2);

	return 0.5f / (Lambda_GGXV + Lambda_GGXL);
}

// height correlated version of smith GGX G
float G_SmithGGXCorrelated(float NdotL, float NdotV, float alphaG)
{
	float alphaG2 = alphaG * alphaG;
	float Lambda_GGXV = NdotL * sqrt((-NdotV * alphaG2 + NdotV) * NdotV + alphaG2);
	float Lambda_GGXL = NdotV * sqrt((-NdotL * alphaG2 + NdotL) * NdotL + alphaG2);
	return 2.0 * NdotL * NdotV / (Lambda_GGXV + Lambda_GGXL);
}

// GGX NDF
float D_GGX(float NdotH, float m)
{
	// Divide by PI is apply later
	float m2 = m * m;
	float f = (NdotH * m2 - NdotH) * NdotH + 1;
	return m2 / (f * f);
}

// D/PI
float D_GGX_Divide_Pi(float NdotH, float roughness)
{
    return D_GGX(NdotH, roughness) / M_PI;
}

// diffuse brdf from disney
float Fr_DisneyDiffuse(
	float NdotV,
	float NdotL,
	float LdotH,
	float linearRoughness)
{
	float energyBias = mix(0.0, 0.5, linearRoughness);
	float energyFactor = mix(1.0, 1.0 / 1.51, linearRoughness);
	float fd90 = energyBias + 2.0 * LdotH*LdotH * linearRoughness;
	vec3 f0 = vec3(1.0f, 1.0f, 1.0f);
	float lightScatter = F_Schlick(f0, fd90, NdotL).r;
	float viewScatter = F_Schlick(f0, fd90, NdotV).r;
	return lightScatter * viewScatter * energyFactor;
}

// calculate specular sum part
vec3 IntegrateCubeLDOnly(
	in vec3 V,
	in vec3 N,
	in float roughness)
{
	vec3 accBrdf = vec3(0);
	float accBrdfWeight = 0;
	for (int i=0; i<sampleCount; ++i)
	{
		vec2 eta = GetSample(i, sampleCount);
		vec3 L;
		vec3 H;
		ImportanceSampleGGXDir(eta, V, N, roughness , H, L);
		float NdotL = dot(N,L);
		if (NdotL >0)
		{
			// Use pre-filtered importance sampling (i.e use lower mipmap
			// level for fetching sample with low probability in order
			// to reduce the variance).
			// (Reference: GPU Gem3)
			//
			// Since we pre-integrate the result for normal direction ,
			// N == V and then NdotH == LdotH. This is why the BRDF pdf
			// can be simplifed from:
			// pdf = D_GGX_Divide_Pi(NdotH , roughness)*NdotH/(4*LdotH);
			// to
			// pdf = D_GGX_Divide_Pi(NdotH , roughness) / 4;
			//
			// The mipmap level is clamped to something lower than 8x8
			// in order to avoid cubemap filtering issues
			//
			// - OmegaS: Solid angle associated to a sample
			// - OmegaP: Solid angle associated to a pixel of the cubemap
			float mipLevel = 0;
			float NdotH = Saturate(dot(N, H));
			float LdotH = Saturate(dot(L, H));
			float pdf = D_GGX_Divide_Pi(NdotH , roughness) * NdotH/(4*LdotH);
			if (pdf > FLOAT_EPSILON) {
				float omegaS = 1.0 / (sampleCount * pdf);
				float omegaP = 4.0 * M_PI / (6.0 * float(gl_NumWorkGroups.x * gl_WorkGroupSize.x * gl_NumWorkGroups.y * gl_WorkGroupSize.y));
				float mipCount = INPUT_ENVMAP_MAX_MIPLEVEL;
				mipLevel = clamp(0.5 * log2(omegaS/omegaP), 0, mipCount);
			}
			vec4 Li = texture(inputEnvmap, L, mipLevel);

			accBrdf += Li.rgb * NdotL;
			accBrdfWeight += NdotL;
		}
	}
	if (FloatEqual(accBrdfWeight, 0.0)) {
		return vec3(0.0);
	}
	return accBrdf * (1.0f / accBrdfWeight);
}

// calculates a normal vector from the specified texture coordinates and cube face.
vec3 TexCoordToDirection(vec2 uv, uint face)
{
    vec3 n = vec3(0.0); // This is a normal vector.
    vec3 t = vec3(0.0); // This is a tangent vector.
    if (face == 0)
    {
        n = vec3(1, 0, 0);
        t = vec3(0, 1, 0);
    }
    else if (face == 1)
    {
        n = vec3(-1, 0, 0);
        t = vec3(0, 1, 0);
    }
    else if (face == 2)
    {
        n = vec3(0, -1, 0);
        t = vec3(0, 0, -1);
    }
    else if (face == 3)
    {
        n = vec3(0, 1, 0);
        t = vec3(0, 0, 1);
    }
    else if (face == 4)
    {
        n = vec3(0, 0, -1);
        t = vec3(0, 1, 0);
    }
    else
    {
        n = vec3(0, 0, 1);
        t = vec3(0, 1, 0);
    }
    // Calculate a binormal vector.
    vec3 b = cross(n, t);
    // Convert the texture coordinates from [0, 1] to [-1, 1] range.
    uv = uv * 2 - vec2(1);
    // Calculate a new normal vector for this pixel (current texture coordinates).
    n = n + t * uv.y + b * uv.x;
    n.y = -n.y;
    n.z = -n.z;
    return normalize(n);
}

void main()
{
	#ifdef CALCULATE_DFG
	#elif CALCULATE_SPECULAR_CUBE_MAP
	#elif CALCULATE_DIFFUSE_CUBE_MAP
	#endif
}