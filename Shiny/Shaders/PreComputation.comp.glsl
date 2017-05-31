#version 450 core
layout (binding = 0, std430) buffer result
{
	int sum;
	int sum2;
};

float saturate(float value)
{
	return clamp(value, 0.0, 1.0);
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
	float energyBias = lerp(0, 0.5, linearRoughness);
	float energyFactor = lerp(1.0, 1.0 / 1.51, linearRoughness);
	float fd90 = energyBias + 2.0 * LdotH*LdotH * linearRoughness;
	vec3 f0 = vec3(1.0f, 1.0f, 1.0f);
	float lightScatter = F_Schlick(f0, fd90, NdotL).r;
	float viewScatter = F_Schlick(f0, fd90, NdotV).r;
	return lightScatter * viewScatter * energyFactor;
}

void importanceSampleCosDir(
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
	float phi = u2 * FB_PI * 2;
	L = vec3(r*cos(phi), r*sin(phi), sqrt(max(0.0f,1.0f-u1)));
	L = normalize(tangentX * L.y + tangentY * L.x + N * L.z);
	NdotL = dot(L,N);
	pdf = NdotL * FB_INV_PI;
}

vec4 integrateDFGOnly(
	in vec3 V,
	in vec3 N,
	in float roughness)
{
	float NdotV = saturate(dot(N, V));
	vec4 acc = 0;
	float accWeight = 0;
	// Compute pre-integration
	Referential referential = createReferential(N);
	for (uint i=0; i<sampleCount; ++i)
	{
		vec2 u = getSample(i, sampleCount);
		vec3 L = 0;
		float NdotH = 0;
		float LdotH = 0;
		float G = 0;
		// See [Karis13] for implementation
		importanceSampleGGX_G(u, V, N, referential , roughness , NdotH , LdotH , L, G);
		// specular GGX DFG preIntegration
		float NdotL = saturate(dot(N, L));
		if (NdotL >0 && G > 0.0)
		{
			float GVis = G * LdotH / (NdotH * NdotV);
			float Fc = pow(1-LdotH , 5.f);
			acc.x += (1-Fc) * GVis;
			acc.y += Fc*GVis;
		}
		// diffuse Disney preIntegration
		u = frac(u + 0.5);
		float pdf;
		// The pdf is not use because it cancel with other terms
		// (The 1/PI from diffuse BRDF and the NdotL from Lambert ’s law).
		importanceSampleCosDir(u, N, L, NdotL , pdf);
		if (NdotL >0)
		{
			float LdotH = saturate(dot(L, normalize(V + L));
			float NdotV = saturate(dot(N, V));
			acc.z += Fr_DisneyDiffuse(NdotV , NdotL , LdotH , sqrt(roughness));
		}
		accWeight += 1.0;
	}
	return acc * (1.0f / accWeight);
}

layout(local_size_x = 32, local_size_y = 32) in;
void main()
{
	atomicAdd(sum, 1);
	atomicAdd(sum2, 2);
}