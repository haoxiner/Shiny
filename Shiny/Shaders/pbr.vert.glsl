#version 450 core
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec4 vertexNormal;
layout(location = 2) in vec2 vertexTexCoord;
layout(location = 3) in vec4 boneID;
layout(location = 4) in vec4 boneWeight;
out vec3 position;
out vec3 normal;
out vec2 texCoord;
layout(binding = 0, std140) uniform StaticConstantBuffer
{
	mat4 viewToProjection;
};
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
void main()
{
	position = (modelToWorld * vec4(vertexPosition.xyz, 1.0)).xyz;
	normal = normalize(modelToWorld * vec4(vertexNormal.xyz, 0.0)).xyz;
	texCoord = vertexTexCoord;

	gl_Position = viewToProjection * worldToView * vec4(position, 1.0);
}