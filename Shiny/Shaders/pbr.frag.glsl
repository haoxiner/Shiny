#version 450 core
#define PI 3.14159265
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

vec4 SamplePanorama(sampler2D panorama, vec3 direction)
{
	vec2 uv = vec2(PI + atan(direction.x, -direction.z), acos(-direction.y)) / vec2(2.0 * PI, PI);
	return texture(panorama, uv);
}

void main()
{
	vec3 omegaI = normalize(-position);
	vec3 omegaO = normalize(-position);//normalize(viewDirection);
	vec3 N = normalize(normal);
	
	// vec3 r = normalize(reflect(-omegaI, n));
	vec3 H = normalize(omegaI + omegaO);

	float diffuse = max(0.0, dot(omegaI, N));
	float specular = pow(max(0.0, dot(N, H)), 1000.0);


	vec3 rd = normalize(reflect(-omegaO, N));
	// vec2 ll = vec2(atan(d.x, d.z) / (2*3.141592654), acos(d.y) / 3.141592654);
	//fragColor = pow(textureLL(envmap, d)/* * vec4(vec3(diffuse * 0.6 + specular * 0.4), 1.0)*/, vec4(1.0/2.2));
	//fragColor = vec4(n * 0.5 + vec3(0.5), 1.0);
	//fragColor = vec4(d * 0.5 + vec3(0.5), 1.0);

	// vec2 ll = vec2(atan(rd.z, rd.x) + PI, acos(-rd.y)) / vec2(2.0 * PI, PI);
	fragColor = pow(SamplePanorama(envmap, rd), vec4(1.0/2.2));
	// fragColor = pow(texture(envmap, texCoord), vec4(1.0/2.2));
	// fragColor = 
	//fragColor = pow(texture(sky, rd), vec4(1.0/2.2));
	// fragColor = vec4(rd * 0.5 + vec3(0.5), 1.0);
	// if (rd.x == 0.0) { 
	// 	fragColor = vec4(1.0, 0.0, 0.0, 1.0);
	// }
}