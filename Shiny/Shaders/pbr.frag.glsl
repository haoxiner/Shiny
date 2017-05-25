#version 450 core
in vec3 position;
in vec3 viewDirection;
in vec3 normal;
in vec2 texCoord;
out vec4 fragColor;
void main()
{
	vec3 omegaI = normalize(vec3(0.0) - position);
	vec3 omegaO = normalize(viewDirection);
	vec3 n = normalize(normal);
	
	vec3 r = normalize(reflect(-omegaI, n));
	vec3 h = normalize(omegaI + omegaO);

	float diffuse = max(0.0, dot(omegaI, n));
	float specular = pow(max(0.0, dot(n, h)), 10.0);

	fragColor = vec4(vec3(diffuse * 0.6 + specular * 0.4), 1.0);
}