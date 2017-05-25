#version 450 core
layout(location = 0) in vec4 vertexAttribute0;
layout(location = 1) in vec4 vertexAttribute1;
out vec3 position;
out vec3 viewDirection;
out vec3 normal;
out vec2 texCoord;
layout(binding = 0, std140) uniform StaticConstantBuffer
{
	mat4 viewToProjection;
};
layout(binding = 1, std140) uniform PerFrameConstantBuffer
{
	vec4 data;
};
layout(binding = 2, std140) uniform PerObjectConstantBuffer
{
	mat4 modelToWorld;
	mat4 worldToView;
};
void main()
{
	position = (modelToWorld * vec4(vertexAttribute0.xyz, 1.0)).xyz;
	vec4 positionInViewSpace = worldToView * vec4(position, 1.0);
	
	viewDirection = normalize(vec3(0.0) - positionInViewSpace.xyz);
	normal = normalize(modelToWorld * vec4(vertexAttribute1.xyz, 0.0)).xyz;
	texCoord = vec2(vertexAttribute0.w, vertexAttribute1.w);

	gl_Position = viewToProjection * positionInViewSpace;
}