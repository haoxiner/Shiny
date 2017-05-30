#version 450 core
in vec3 position;
// in vec3 viewDirection;
in vec3 normal;
in vec2 texCoord;
out vec4 fragColor;
layout(binding = 1, std140) uniform PerFrameConstantBuffer
{
	vec4 data;
	mat4 worldToView;
};
layout(binding = 0) uniform sampler2D envmap;
//layout(binding = 1) uniform samplerCube sky;
#define PI 3.14159265
#define TWO_PI (2.0 * PI)
vec4 textureLL(sampler2D ll, vec3 d)
{
	float phi = acos(-d.y);
	float theta = atan(1.0 * d.x, d.z) + PI;
	vec2 uv = vec2(theta / TWO_PI, phi / PI);
	return texture(ll, uv);
}
void main()
{
	vec3 omegaI = normalize(vec3(0.0) - position);
	vec3 omegaO = normalize(vec3(0.0) - position);//normalize(viewDirection);
	vec3 n = normalize(normal);
	
	vec3 r = normalize(reflect(-omegaI, n));
	vec3 h = normalize(omegaI + omegaO);

	float diffuse = max(0.0, dot(omegaI, n));
	float specular = pow(max(0.0, dot(n, h)), 1000.0);


	vec3 rd = normalize(reflect(-omegaO, n));
	// vec2 ll = vec2(atan(d.x, d.z) / (2*3.141592654), acos(d.y) / 3.141592654);
	//fragColor = pow(textureLL(envmap, d)/* * vec4(vec3(diffuse * 0.6 + specular * 0.4), 1.0)*/, vec4(1.0/2.2));
	//fragColor = vec4(n * 0.5 + vec3(0.5), 1.0);
	//fragColor = vec4(d * 0.5 + vec3(0.5), 1.0);

	// vec2 ll = vec2(atan(rd.z, rd.x) + PI, acos(-rd.y)) / vec2(2.0 * PI, PI);
	vec2 ll = vec2(PI + atan(rd.x, -rd.z), acos(-rd.y)) / vec2(2.0 * PI, PI);
	fragColor = pow(texture(envmap, ll), vec4(1.0/2.2));
	//fragColor = pow(texture(sky, rd), vec4(1.0/2.2));
	// fragColor = vec4(rd * 0.5 + vec3(0.5), 1.0);
	// if (rd.x == 0.0) {
	// 	fragColor = vec4(1.0, 0.0, 0.0, 1.0);
	// }
}