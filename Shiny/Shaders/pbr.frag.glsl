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
layout(binding = 2, std140) uniform PerObjectConstantBuffer
{
	mat4 modelToWorld;
	vec4 material0;
};

layout(binding = 0) uniform sampler2D dfgMap;
layout(binding = 1) uniform samplerCube diffuseEnvmap;
layout(binding = 2) uniform samplerCube specularEnvmap;

layout(binding = 3) uniform sampler2D baseColorMap;
layout(binding = 4) uniform sampler2D roughnessMap;
layout(binding = 5) uniform sampler2D metallicMap;

float Saturate(float value)
{
	return clamp(value, 0.0, 1.0);
}

vec4 SamplePanorama(sampler2D panorama, vec3 direction)
{
	vec2 uv = vec2(PI + atan(direction.x, -direction.z), acos(-direction.y)) / vec2(2.0 * PI, PI);
	return texture(panorama, uv);
}

vec4 SamplePanorama(sampler2D panorama, vec3 direction, float mipmapLevel)
{
	vec2 uv = vec2(PI + atan(direction.x, -direction.z), acos(-direction.y)) / vec2(2.0 * PI, PI);
	return textureLod(panorama, uv, mipmapLevel);
}

vec3 GetDiffuseDominantDir(vec3 N, vec3 V, float NdotV , float roughness)
{
	float a = 1.02341 * roughness - 1.51174;
	float b = -0.511705 * roughness + 0.755868;
	float lerpFactor = Saturate((NdotV * a + b) * roughness);
	// The result is not normalized as we fetch in a cubemap
	return mix(N, V, lerpFactor);
}

vec3 EvaluateIBLDiffuse(vec3 N, vec3 V, float NdotV , float alphaG)
{
	// vec3 dominantN = GetDiffuseDominantDir(N, V, NdotV , roughness);
	vec3 dominantN = N;
	vec3 diffuseLighting = texture(diffuseEnvmap, dominantN).xyz;
	float diffF = texture(dfgMap, vec2(NdotV , alphaG)).z;
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

// This is an accurate fitting of the specular peak,
// but due to other approximation in our decomposition it doesn ’t perform well
vec3 GetSpecularDominantDir(vec3 N, vec3 R, float NdotV , float roughness)
{
	float lerpFactor = pow(1 - NdotV , 10.8649) * (1 - 0.298475 * log(39.4115 - 39.0029 *
	roughness)) + 0.298475 * log(39.4115 - 39.0029 * roughness);
	// The result is not normalized as we fetch in a cubemap
	return mix(N, R, Saturate(lerpFactor));
}
float LinearRoughnessToMipLevel(float linearRoughness, float maxMipLevel)
{
	return sqrt(linearRoughness) * maxMipLevel;
	// return linearRoughness * maxMipLevel;
}
vec3 EvaluateIBLSpecular(vec3 N, vec3 V, float NdotV , float alphaG, float roughness, vec3 f0, vec3 f90)
{
	// vec3 R = 2 * dot( V, N ) * N - V;
	vec3 R = reflect(-V, N);
	vec3 dominantR = GetSpecularDominantDir(N, R, alphaG);
	// if (material0.z == 0.0) {
	// 	dominantR = R;
	// }
	// vec3 dominantR = GetSpecularDominantDir(N, R, linearRoughness);

	// Rebuild the function
	// L . D. ( f0.Gv.(1-Fc) + Gv.Fc ) . cosTheta / (4 . NdotL . NdotV)
	NdotV = max(NdotV , 0.5/128.0);

	// vec3 dominantR = GetSpecularDominantDir(N, R, NdotV, roughness);

	float mipCount = 6;
	
	float mipLevel = LinearRoughnessToMipLevel(roughness , mipCount);
	vec3 preLD = textureLod(specularEnvmap , dominantR , mipLevel).rgb;
	// vec3 preLD = SamplePanorama(specularEnvmap, dominantR).xyz;

	// Sample pre-integrate DFG
	// Fc = (1-H.L)^5
	// PreIntegratedDFG.r = Gv.(1-Fc)
	// PreIntegratedDFG.g = Gv.Fc
	// vec2 preDFG = DFG.SampleLevel(sampler , float2(NdotV , roughness), 0).xy;
	vec2 preDFG = texture(dfgMap, vec2(NdotV, alphaG)).xy;

	// LD . ( f0.Gv.(1-Fc) + Gv.Fc.f90 )
	return preLD * (f0 * preDFG.x + f90 * preDFG.y);
}

vec3 ApproximationSRgbToLinear(in vec3 sRGBCol)
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

float A = 0.15;
float B = 0.50;
float C = 0.10;
float D = 0.20;
float E = 0.02;
float F = 0.30;
float W = 11.2;

vec3 Uncharted2Tonemap(vec3 x) {
   return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
}

//Based on Filmic Tonemapping Operators http://filmicgames.com/archives/75
vec3 TonemapUncharted2(vec3 color) {
    float ExposureBias = 2.0;
    vec3 curr = Uncharted2Tonemap(ExposureBias * color);

    vec3 whiteScale = 1.0 / Uncharted2Tonemap(vec3(W));
    return curr * whiteScale;
}

vec3 F_Schlick(in vec3 f0, in vec3 f90, in float u)
{
	return f0 + (f90 - f0) * pow(1.f - u, 5.f);
}

float V_SmithGGXCorrelated(float NdotL , float NdotV , float alphaG)
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

	return 0.5 / (Lambda_GGXV + Lambda_GGXL);
}

float D_GGX(float NdotH , float m)
{
	// Divide by PI is apply later
	float m2 = m * m;
	float f = (NdotH * m2 - NdotH) * NdotH + 1;
	return m2 / (f * f);
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
	float lightScatter = F_Schlick(f0, vec3(fd90), NdotL).r;
	float viewScatter = F_Schlick(f0, vec3(fd90), NdotV).r;
	return lightScatter * viewScatter * energyFactor;
}


void main()
{
	fragColor = vec4(0.0);
	vec3 L = normalize(-position);
	vec3 V = normalize(-position);
	vec3 N = normalize(normal);
	
	vec2 uv = texCoord * 4.0;

	float roughness = texture(roughnessMap, uv).r;
	float alphaG = roughness * roughness;
	float metallic = 1.0;//texture(metallicMap, uv).r * material0.y;
	vec3 baseColor = texture(baseColorMap, uv).rgb;

	// roughness = 0.9;
	// metallic = 1.0;
	// alphaG = roughness * roughness;
	// baseColor = vec3(1.0, 1.0, 1.0);

	vec3 reflectance = vec3(0.5);
	vec3 diffuseColor = baseColor * (1.0 - metallic);

	vec3 f0 = 0.16 * reflectance * reflectance * (1.0 - metallic) + baseColor * metallic;
	vec3 f90 = vec3(1.0);//vec3(Saturate(50.0 * dot(f0, vec3(0.33))));

	float NdotV = abs(dot(N, V));
	vec3 specular = EvaluateIBLSpecular(N, V, NdotV, alphaG, roughness, f0, f90);
	vec3 diffuse = diffuseColor * INV_PI * EvaluateIBLDiffuse(N, V, NdotV, alphaG);
	fragColor.xyz += (diffuse + specular) * 0.25;

////////////////////// Test Analytic Light ///////////////////////
	NdotV = NdotV + 1e-5; // avoid artifact
	vec3 pointLight = vec3(100.0,100.0,100.0) - position;
	L = normalize(pointLight);
	vec3 H = normalize(V + L);
	float LdotH = Saturate(dot(L, H));
	float NdotH = Saturate(dot(N, H));
	float NdotL = Saturate(dot(N, L));
	// Specular BRDF
	vec3 F = F_Schlick(f0, f90, LdotH);
	float Vis = V_SmithGGXCorrelated(NdotV , NdotL , alphaG);
	float D = D_GGX(NdotH , alphaG);
	vec3 Fr = D * F * Vis / PI;
	// Diffuse BRDF
	float Fd = Fr_DisneyDiffuse(NdotV , NdotL , LdotH , roughness).r / PI;
	
	fragColor.xyz += vec3(10000, 10000, 10000) * (Fr + diffuseColor * Fd) / dot(pointLight, pointLight);
//////////////////////////////////////////////////////////////////

	float exposure = 2.5;
	fragColor *= exposure;
	fragColor.xyz = TonemapUncharted2(fragColor.xyz);
	fragColor.xyz = ApproximationLinearToSRGB(fragColor.xyz);
	fragColor.w = 1.0;
}