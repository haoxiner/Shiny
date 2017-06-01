#version 450 core
#define PI 3.1415926535897932384626433832795
#define INV_PI (1.0/PI)
#define TWO_PI (2.0 * PI)

in vec3 position;
in vec3 normal;
in vec2 texCoord;
out vec4 fragColor;
layout(binding = 1, std140) uniform PerFrameConstantBuffer
{
	vec4 data;
	mat4 worldToView;
};
layout(binding = 0) uniform sampler2D envmap;
layout(binding = 1) uniform sampler2D dfgMap;

float Saturate(float value)
{
	return clamp(value, 0.0, 1.0);
}

vec4 SamplePanorama(sampler2D panorama, vec3 direction)
{
	vec2 uv = vec2(PI + atan(direction.x, -direction.z), acos(-direction.y)) / vec2(2.0 * PI, PI);
	return texture(panorama, uv);
}

vec3 GetDiffuseDominantDir(vec3 N, vec3 V, float NdotV , float roughness)
{
	float a = 1.02341 * roughness - 1.51174;
	float b = -0.511705 * roughness + 0.755868;
	float lerpFactor = Saturate((NdotV * a + b) * roughness);
	// The result is not normalized as we fetch in a cubemap
	return mix(N, V, lerpFactor);
}

vec4 EvaluateIBLDiffuse(vec3 N, vec3 V, float NdotV , float roughness)
{
	vec3 dominantN = GetDiffuseDominantDir(N, V, NdotV , roughness);
	vec4 diffuseLighting = SamplePanorama(envmap, dominantN);
	float diffF = texture(dfgMap, vec2(NdotV , roughness)).z;
	return diffuseLighting * diffF;
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

void main()
{
	vec3 L = normalize(-position);
	vec3 V = normalize(-position);//normalize(viewDirection);
	vec3 N = normalize(normal);
	
	// vec3 r = normalize(reflect(-L, n));
	vec3 H = normalize(L + V);

	float diffuse = max(0.0, dot(L, N));
	float specular = pow(max(0.0, dot(N, H)), 1000.0);


	vec3 rd = normalize(reflect(-V, N));
	// vec2 ll = vec2(atan(d.x, d.z) / (2*3.141592654), acos(d.y) / 3.141592654);
	//fragColor = pow(textureLL(envmap, d)/* * vec4(vec3(diffuse * 0.6 + specular * 0.4), 1.0)*/, vec4(1.0/2.2));
	//fragColor = vec4(n * 0.5 + vec3(0.5), 1.0);
	//fragColor = vec4(d * 0.5 + vec3(0.5), 1.0);

	// vec2 ll = vec2(atan(rd.z, rd.x) + PI, acos(-rd.y)) / vec2(2.0 * PI, PI);
	// fragColor = pow(SamplePanorama(envmap, N) * INV_PI, vec4(1.0/2.2));
	fragColor = pow(INV_PI * EvaluateIBLDiffuse(N, V, dot(N, V), 0.5), vec4(1.0/2.2));
	fragColor.w = 1.0;
	
}