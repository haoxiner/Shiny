#version 450 core
in vec3 position;
out vec4 fragColor;
layout(binding = 0) uniform sampler2D envmap;
void main()
{
	fragColor = pow(texture(envmap, texCoord), vec4(1.0/2.2));
}