#version 450 core
layout(location = 0) in vec4 vertexAttribute0;
out vec3 position;
layout(binding = 1, std140) uniform PerFrameConstantBuffer
{
	vec4 data;
	mat4 worldToView;
};
void main()
{
	position = mat3(worldToView) * vertexAttribute0.xyz;
	gl_Position = vertexAttribute0;
}