// #version 450 core
// layout(local_size_x = 8, local_size_y = 8) in;
#define PI 3.1415926535897932384626433832795
#define INV_PI (1.0/PI)

layout (binding = 0, std140) uniform InputBuffer
{
	vec4 inputArg0;
	vec4 inputArg1;
};
layout (binding = 0) uniform samplerCube inputEnvmap;
layout (rgba32f, binding = 0) uniform image2D outputEnvmap;

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
vec4 SampleCubeMap(samplerCube cubemap, vec3 direction)
{
	return texture(cubemap, direction);
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

vec3 ImportanceSampleGGX( vec2 Xi, float Roughness, vec3 N )
{
	float a = Roughness * Roughness;
	float Phi = 2 * PI * Xi.x;
	float CosTheta = sqrt( (1 - Xi.y) / ( 1 + (a*a - 1) * Xi.y ) );
	float SinTheta = sqrt( 1 - CosTheta * CosTheta );
	vec3 H;
	H.x = SinTheta * cos( Phi );
	H.y = SinTheta * sin( Phi );
	H.z = CosTheta;
	vec3 UpVector = abs(N.z) < 0.999 ? vec3(0,0,1) : vec3(1,0,0);
	vec3 TangentX = normalize( cross( UpVector, N ) );
	vec3 TangentY = cross( N, TangentX );
	// Tangent to world space
	return TangentX * H.x + TangentY * H.y + N * H.z;
}
vec3 PrefilterEnvMap( float Roughness, vec3 R )
{
	vec3 N = R;
	vec3 V = R;
	vec3 PrefilteredColor = vec3(0);
	const uint NumSamples = uint(inputArg0.w);
	float TotalWeight = 0.0;
	for( uint i = 0; i < NumSamples; i++ )
	{
		vec2 Xi = Hammersley( i, NumSamples );
		vec3 H = ImportanceSampleGGX( Xi, Roughness, N );
		vec3 L = 2 * dot( V, H ) * H - V;
		float NoL = Saturate( dot( N, L ) );
		if( NoL > 0 )
		{
			PrefilteredColor += SampleCubeMap(inputEnvmap, L).rgb * NoL;
			TotalWeight += NoL;
		}
	}
	return TotalWeight > 0.0 ? (PrefilteredColor / TotalWeight) : PrefilteredColor;
}
float D_GGX(float NdotH, float m)
{
	// Divide by PI is apply later
	float m2 = m * m;
	float f = (NdotH * m2 - NdotH) * NdotH + 1;
	return m2 / (f * f);
}
float D_GGX_Divide_Pi(float NdotH, float roughness)
{
    return D_GGX(NdotH, roughness) / PI;
}


void main()
{
	vec2 texCoord = (vec2(0.5) + vec2(gl_GlobalInvocationID.xy)) / inputArg0.xy;
	vec3 d = TexCoordToDirection(texCoord);
	// vec4 diffuse = IntegrateDiffuseCube(d);
	float roughness = inputArg1.y / inputArg1.z;
	vec3 pixel = PrefilterEnvMap(roughness, d);
	
	// imageStore(outputEnvmap, ivec2(gl_GlobalInvocationID.xy),
	// 	texelFetch(inputEnvmap, ivec2(gl_GlobalInvocationID.xy), 0));
	imageStore(outputEnvmap, ivec2(gl_GlobalInvocationID.xy),
		vec4(pixel, 1.0));
}