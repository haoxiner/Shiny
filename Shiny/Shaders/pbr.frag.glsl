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
layout(binding = 2) uniform sampler2D specularEnvmap;
// layout(binding = 2) uniform samplerCube cubemap;
const uint NumSamples = 1024;
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

vec3 EvaluateIBLDiffuse(vec3 N, vec3 V, float NdotV , float roughness)
{
	vec3 dominantN = GetDiffuseDominantDir(N, V, NdotV , roughness);
	vec3 diffuseLighting = SamplePanorama(envmap, dominantN).xyz;
	float diffF = texture(dfgMap, vec2(NdotV , roughness)).z;
	return diffuseLighting * diffF;
}
// We have a better approximation of the off specular peak
// but due to the other approximations we found this one performs better.
// N is the normal direction
// R is the mirror vector
// This approximation works fine for G smith correlated and uncorrelated
vec3 GetSpecularDominantDir(vec3 N, vec3 R, float roughness)
{
	float smoothness = Saturate(1.0 - roughness);
	float lerpFactor = smoothness * (sqrt(smoothness) + roughness);
	// The result is not normalized as we fetch in a cubemap
	return mix(N, R, lerpFactor);
}
vec3 EvaluateIBLSpecular(vec3 N, vec3 V, float NdotV , float roughness)
{
	vec3 R = 2 * dot( V, N ) * N - V;
	vec3 dominantR = GetSpecularDominantDir(N, R, roughness);

	// Rebuild the function
	// L . D. ( f0.Gv.(1-Fc) + Gv.Fc ) . cosTheta / (4 . NdotL . NdotV)
	NdotV = max(NdotV , 0.5/128.0);
	// float mipLevel = linearRoughnessToMipLevel(linearRoughness , mipCount);
	// vec3 preLD = specularLD.SampleLevel(sampler , dominantR , mipLevel).rgb;
	vec3 preLD = SamplePanorama(specularEnvmap, dominantR).xyz;

	// Sample pre-integrate DFG
	// Fc = (1-H.L)^5
	// PreIntegratedDFG.r = Gv.(1-Fc)
	// PreIntegratedDFG.g = Gv.Fc
	// vec2 preDFG = DFG.SampleLevel(sampler , float2(NdotV , roughness), 0).xy;
	vec2 preDFG = texture(dfgMap, vec2(NdotV, roughness)).xy;

	float f0 = 1.0;
	float f90 = Saturate(50.0 * dot(f0 , 0.33));
	// LD . ( f0.Gv.(1-Fc) + Gv.Fc.f90 )
	return preLD * (f0 * preDFG.x + f90 * preDFG.y);
}

vec3 approximationSRgbToLinear(in vec3 sRGBCol)
{
	return pow(sRGBCol , vec3(2.2));
}

vec3 ApproximationLinearToSRGB(in vec3 linearCol)
{
	return pow(linearCol , vec3(1.0 / 2.2));
}

// vec3 accurateSRGBToLinear(in vec3 sRGBCol)
// {
// 	vec3 linearRGBLo = sRGBCol / 12.92;
// 	vec3 linearRGBHi = pow((sRGBCol + vec3(0.055)) / 1.055, vec3(2.4));
// 	vec3 linearRGB = (sRGBCol <= vec3(0.04045)) ? linearRGBLo : linearRGBHi;
// 	return linearRGB;
// }

vec3 accurateLinearToSRGB(in vec3 linearCol)
{
	vec3 sRGBLo = linearCol * 12.92;
	vec3 sRGBHi = (pow(abs(linearCol), vec3(1.0/2.4)) * 1.055) - vec3(0.055);
	vec3 sRGB = (linearCol.x <= 0.0031308 || linearCol.y <= 0.0031308 || linearCol.z <= 0.0031308 ) ? sRGBLo : sRGBHi;
	return sRGB;
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
	float roughness = 0.1;
	fragColor.xyz = EvaluateIBLSpecular(N, V, dot(N, V), roughness) * 0.9 + 0.1 * INV_PI * EvaluateIBLDiffuse(N, V, dot(N, V), roughness);
	fragColor = pow(fragColor * (vec4(212,175,55,255) / 255.0), vec4(1.0/1.0));
	fragColor.xyz = ApproximationLinearToSRGB(fragColor.xyz);
	fragColor.w = 1.0;
}