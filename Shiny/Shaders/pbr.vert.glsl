#version 450 core
layout(location = 0) in vec4 vertexAttribute0;
layout(location = 1) in vec4 vertexAttribute1;
out vec3 position;
out vec3 normal;
out vec2 texCoord;
void main()
{
	position = vertexAttribute0.xyz;
	normal = vertexAttribute1.xyz;
	texCoord = vec2(vertexAttribute0.w, vertexAttribute1.w);
	gl_Position = vec4(vertexAttribute0.xyz, 1.0);
}