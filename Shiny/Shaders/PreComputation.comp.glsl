#version 450 core
#define PI 3.1415926535897932384626433832795
#define INV_PI (1.0/PI)
const int sampleCount = 1024 * 16;
layout (binding = 0, std140) uniform InputBuffer
{
	vec4 inputArg0;
};
layout (binding = 0) uniform sampler2D inputEnvmap;
layout (rgba32f, binding = 0) uniform image2D outputEnvmap;

vec4 SamplePanorama(sampler2D panorama, vec3 direction)
{
	vec2 uv = vec2(PI + atan(direction.x, -direction.z), acos(-direction.y)) / vec2(2.0 * PI, PI);
	return texture(panorama, uv);
}

vec3 TexCoordToDirection(vec2 texCoord)
{
	float u = texCoord.s;
	float v = texCoord.t;
	float theta = v * PI;
	float phi = (2 * u - 1) * PI;
	return vec3(sin(theta) * sin(phi), -cos(theta), -sin(theta) * cos(phi));
}

float Saturate(float value)
{
	return clamp(value, 0.0, 1.0);
}

vec2 Hammersley(uint i, uint N)
{
    float ri = float(bitfieldReverse(i)) * 2.3283064365386963e-10;
    return vec2(float(i) / float(N), ri);
}

vec2 GetSample(int i, int total)
{
	return Hammersley(uint(i), uint(total));
}

vec3 F_Schlick(in vec3 f0, in float f90, in float u)
{
	return f0 + (f90 - f0) * pow(1.f - u, 5.f);
}

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
float G_SmithGGXCorrelated(float NdotL, float NdotV, float alphaG)
{
	float alphaG2 = alphaG * alphaG;
	float Lambda_GGXV = NdotL * sqrt((-NdotV * alphaG2 + NdotV) * NdotV + alphaG2);
	float Lambda_GGXL = NdotV * sqrt((-NdotL * alphaG2 + NdotL) * NdotL + alphaG2);
	return 2.0 * NdotL * NdotV / (Lambda_GGXV + Lambda_GGXL);
}
float D_GGX(float NdotH, float m)
{
	// Divide by PI is apply later
	float m2 = m * m;
	float f = (NdotH * m2 - NdotH) * NdotH + 1;
	return m2 / (f * f);
}

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

void ImportanceSampleCosDir(
	in vec2 u,
	in vec3 N,
	out vec3 L,
	out float NdotL,
	out float pdf)
{
	// Local referencial
	vec3 upVector = abs(N.z) < 0.999 ? vec3(0,0,1) : vec3(1,0,0);
	vec3 tangentX = normalize( cross( upVector , N ) );
	vec3 tangentY = cross( N, tangentX );
	float u1 = u.x;
	float u2 = u.y;
	float r = sqrt(u1);
	float phi = u2 * PI * 2;
	L = vec3(r*cos(phi), r*sin(phi), sqrt(max(0.0f,1.0f-u1)));
	L = normalize(tangentX * L.y + tangentY * L.x + N * L.z);
	NdotL = dot(L,N);
	pdf = NdotL * INV_PI;
}
struct Referential
{
	vec3 upVector;
	vec3 tangentX;
	vec3 tangentY;
};
Referential CreateReferential(vec3 N)
{
	Referential referential;
	referential.upVector = abs(N.z) < 0.999 ? vec3(0,0,1) : vec3(1,0,0);
	referential.tangentX = normalize( cross( referential.upVector, N ) );
	referential.tangentY = cross( N, referential.tangentX );
	return referential;
}
vec3 ImportanceSampleGGX( vec2 Xi, float Roughness, vec3 N, Referential referential )
{
	float a = Roughness * Roughness;
	float Phi = 2 * PI * Xi.x;
	float CosTheta = sqrt( (1 - Xi.y) / ( 1 + (a*a - 1) * Xi.y ) );
	float SinTheta = sqrt( 1 - CosTheta * CosTheta );
	vec3 H;
	H.x = SinTheta * cos( Phi );
	H.y = SinTheta * sin( Phi );
	H.z = CosTheta;
	// Tangent to world space
	return referential.tangentX * H.x + referential.tangentY * H.y + N * H.z;
}
void ImportanceSampleGGX_G(
	in vec2 u, in vec3 V, in vec3 N,
	in Referential referential, in float roughness,
	out float NdotH, out float LdotH, out vec3 L, out float G)
{
	vec3 H = ImportanceSampleGGX(u, roughness, N, referential);
	L = 2 * dot( V, H ) * H - V;
	NdotH = Saturate(dot(N, H));
	LdotH = Saturate(dot(L, H));
	float NdotL = Saturate(dot(N, L));
	float NdotV = Saturate(dot(N, V)); 
	G = G_SmithGGXCorrelated(NdotL, NdotV, roughness);
}
vec4 IntegrateDFGOnly(
	in vec3 V,
	in vec3 N,
	in float roughness)
{
	float NdotV = Saturate(dot(N, V));
	vec4 acc = vec4(0.0);
	float accWeight = 0.0;
	// Compute pre-integration
	Referential referential = CreateReferential(N);
	for (int i=0; i<sampleCount; ++i)
	{
		vec2 u = GetSample(i, sampleCount);
		vec3 L = vec3(0);
		float NdotH = 0;
		float LdotH = 0;
		float G = 0;
		// See [Karis13] for implementation
		ImportanceSampleGGX_G(u, V, N, referential , roughness , NdotH , LdotH , L, G);
		// specular GGX DFG preIntegration
		float NdotL = Saturate(dot(N, L));
		if (NdotL >0 && G > 0.0)
		{
			float GVis = G * LdotH / (NdotH * NdotV);
			float Fc = pow(1-LdotH , 5.f);
			acc.x += (1-Fc) * GVis;
			acc.y += Fc*GVis;
		}
		// diffuse Disney preIntegration
		u = fract(u + 0.5);
		float pdf;
		// The pdf is not use because it cancel with other terms
		// (The 1/PI from diffuse BRDF and the NdotL from Lambert ’s law).
		ImportanceSampleCosDir(u, N, L, NdotL , pdf);
		if (NdotL >0)
		{
			float LdotH = Saturate(dot(L, normalize(V + L)));
			float NdotV = Saturate(dot(N, V));
			acc.z += Fr_DisneyDiffuse(NdotV , NdotL , LdotH , sqrt(roughness));
		}
		accWeight += 1.0;
	}
	return acc * (1.0f / accWeight);
}

vec4 IntegrateDiffuseCube(in vec3 N)
{
	vec3 accBrdf = vec3(0);
	for (int i=0; i<sampleCount; ++i)
	{
		vec2 eta = GetSample(i, sampleCount);
		vec3 L;
		float NdotL;
		float pdf;
		// see reference code in appendix
		ImportanceSampleCosDir(eta, N, L, NdotL , pdf);
		if (NdotL >0)
			accBrdf += SamplePanorama(inputEnvmap, L).rgb;//IBLCube.Sample(incomingLightSampler , L).rgb;
	}
	return vec4(accBrdf * (1.0 / sampleCount), 1.0);
}

void OutputDiffuse()
{
	vec2 texCoord = (vec2(gl_GlobalInvocationID.xy)) / inputArg0.xy;
	vec3 d = TexCoordToDirection(texCoord);
	vec4 diffuse = IntegrateDiffuseCube(d);
	imageStore(outputEnvmap, ivec2(gl_GlobalInvocationID.xy), diffuse);
}

void OutputDFG()
{
	vec2 texCoord = (vec2(gl_GlobalInvocationID.xy) + vec2(0.5)) / inputArg0.xy;
	float NoV = texCoord.x;
	vec3 V;
	V.x = sqrt( 1.0 - NoV * NoV ); // sin
	V.y = 0;
	V.z = NoV; // cos
	vec4 dfg = IntegrateDFGOnly(vec3(0.0,0.0,1.0), V, texCoord.y);
	imageStore(outputEnvmap, ivec2(gl_GlobalInvocationID.xy),
		dfg);
}

layout(local_size_x = 32, local_size_y = 32) in;
void main()
{
	OutputDiffuse();
}