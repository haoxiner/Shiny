#version 450 core
in vec3 position;
in vec3 normal;
out vec4 fragColor;
void main()
{
	fragColor = vec4(vec3(0.5) + normal * 0.5, 1.0);
}