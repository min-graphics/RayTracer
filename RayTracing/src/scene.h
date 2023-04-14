#pragma once
#include<vector>

#include"glm/glm.hpp"

struct Material
{
	glm::vec3 Albedo{ 1.0f };
	float Roughness = 1.0f;
	float Metallic = 0.0f;
};

struct Sphere
{
	glm::vec3 Position{ 0.0f };
	float Radius = 0.5f;

	//Material Mat;要保存材质序号
	int MaterialIndex = 0;

};

struct Scene
{
	std::vector<Sphere> Spheres;

	std::vector<Material> Materials;//不要将材质与物体强绑定，申请一个材质数组，这样的话，可以给物体进行各种材质切换。这样的话在物体结构体内保存的也不是材质，而是材质的序号

};