#version 450 core
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec4 vertexNormal;
layout(location = 2) in vec2 vertexTexCoord;
out vec3 position;
out vec3 normal;
out vec2 texCoord;
layout(binding = 0, std140) uniform StaticConstantBuffer
{
	mat4 viewToProjectionForYup;
	mat4 viewToProjectionForZup;
};
layout(binding = 1, std140) uniform PerFrameConstantBuffer
{
	vec4 cameraPosition;
	mat4 worldToView;
};
layout(binding = 2, std140) uniform PerObjectConstantBuffer
{
	mat4 modelToWorld;
	vec4 material0;
	vec4 animationState;
};
void main()
{
	position = vertexPosition;
	normal = normalize(vertexNormal.xyz);

	position = (modelToWorld * vec4(position, 1.0)).xyz;
	normal = normalize(modelToWorld * vec4(normal, 0.0)).xyz;
	texCoord = vertexTexCoord;

	gl_Position = viewToProjectionForZup * worldToView * vec4(position, 1.0);
}