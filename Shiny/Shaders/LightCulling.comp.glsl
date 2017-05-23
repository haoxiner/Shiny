#version 450 core
layout (binding = 0, std430) buffer result
{
	int sum;
};
layout(local_size_x = 32, local_size_y = 32) in;
void main()
{
	atomicAdd(sum, 1);
}