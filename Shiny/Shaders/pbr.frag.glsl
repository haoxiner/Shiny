#version 450 core
in vec3 position;
in vec3 viewDirection;
in vec3 normal;
in vec2 texCoord;
out vec4 fragColor;
layout(binding = 1, std140) uniform PerFrameConstantBuffer
{
	vec4 data;
	mat4 worldToView;
};
layout(binding = 0) uniform sampler2D envmap;
void main()
{
	vec3 omegaI = normalize(vec3(0.0) - position);
	vec3 omegaO = normalize(viewDirection);
	vec3 n = normalize(normal);
	
	vec3 r = normalize(reflect(-omegaI, n));
	vec3 h = normalize(omegaI + omegaO);

	float diffuse = max(0.0, dot(omegaI, n));
	float specular = pow(max(0.0, dot(n, h)), 10.0);

	fragColor = pow(texture(envmap, texCoord) * vec4(vec3(diffuse * 0.6 + specular * 0.4), 1.0), vec4(1.0/2.2));
}