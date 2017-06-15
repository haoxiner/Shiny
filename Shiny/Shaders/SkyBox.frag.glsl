#version 450 core
in vec3 cubemapDirection;
out vec4 fragColor;
layout(binding = 0) uniform samplerCube envmap;
layout(binding = 1, std140) uniform PerFrameConstantBuffer
{
	vec4 data;
	mat4 worldToView;
};
vec4 SampleCubemapForZup(samplerCube cubemap, vec3 direction)
{
	vec4 color = texture(cubemap, vec3(direction.x, direction.z, -direction.y));
	return color;
}
void main()
{
	fragColor = pow(SampleCubemapForZup(envmap, cubemapDirection), vec4(1.0/2.2));
}