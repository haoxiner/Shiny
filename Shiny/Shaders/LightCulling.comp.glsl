#version 450 core
layout (binding = 0, std430) buffer result
{
	int sum;
	int sum2;
};
layout(local_size_x = 32, local_size_y = 32) in;
void main()
{
	atomicAdd(sum, 1);
	atomicAdd(sum2, 2);
}