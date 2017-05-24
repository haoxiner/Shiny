#version 450 core
in vec3 position;
in vec3 normal;
in vec2 texCoord;
out vec4 fragColor;
void main()
{
	fragColor = vec4(vec3(texCoord, 0.0), 1.0);
}