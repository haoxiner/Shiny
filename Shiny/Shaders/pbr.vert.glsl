#version 450 core
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec4 vertexNormal;
layout(location = 2) in vec2 vertexTexCoord;
layout(location = 3) in uvec4 boneID;
layout(location = 4) in vec4 boneWeight;
out vec3 position;
out vec3 normal;
out vec2 texCoord;
layout(binding = 0, std140) buffer AnimationBuffer
{
	// 3 vec4 per frame per bone
	// max 100 bone
	// max 100 frame
	// max 100 animation
	// less than 50MB GPU Memory
	// readonly vec4 animationFrame[3 * 100 * 100 * 100];
	readonly vec4 animationFrame[];
};
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
	int animationFrameID;
};
void main()
{
	position = vec3(0.0);
	normal = vec3(0.0);

	vec4 P = vec4(vertexPosition, 1.0);
	vec4 N = vec4(normalize(vertexNormal.xyz), 0.0);
	uint frameID = 0;
	for (int i = 0; i < 4; i++) {
		vec4 frames[3];
		for (int j = 0; j < 3; j++) {
			frames[j] = animationFrame[frameID + boneID[i]*3 + j];
		}
		position += boneWeight[i] * vec3(dot(P, frames[0]), dot(P, frames[1]), dot(P, frames[2]));
		normal += boneWeight[i] * vec3(dot(N, frames[0]), dot(N, frames[1]), dot(N, frames[2]));
	}

	position = (modelToWorld * vec4(position, 1.0)).xyz;
	normal = normalize(modelToWorld * vec4(normal, 0.0)).xyz;
	texCoord = vertexTexCoord;

	gl_Position = viewToProjection * worldToView * vec4(position, 1.0);
}