#pragma once
#include"Walnut/Image.h"
#include"camera.h"

#include"Ray.h"
#include"scene.h"

#include<glm/glm.hpp>
#include<memory>

//输入是场景描述，输出是一个2D的图像，保存渲染器生成的像素。
class Renderer
{
public:
	struct Settings
	{
		bool Accumulate = true;
	};
public:
	Renderer() = default;

	void OnResize(uint32_t width, uint32_t height);
	void Render(const Camera& m_camera,const Scene& scene);

	std::shared_ptr<Walnut::Image> GetFinalImage()const
	{
		return m_FinalImage;
	}

	void ResetFrameIndex() { m_FrameIndex = 1; };

	Settings& GetSettings() { return m_Settings; };
private:

	struct HitPayload
	{
		float HitDistance;
		glm::vec3 WorldPosition;
		glm::vec3 WorldNormal;

		uint32_t ObjectIndex;
	};

	glm::vec4 PerPixel(uint32_t x,uint32_t y);//rayGen shader,在DX12或者vulkan中，被每个尝试渲染的像素调用，并且在该着色器中，决定是否要潜在的每个像素投射多条光线

	////输入一个坐标，返回一个颜色.
	//glm::vec4 TraceRay(const Ray& ray,const Scene& scene);

	//新的TraceRay函数输入应该是射线，输出是击中物体或者没有击中物体的载荷Payload,也就是我们需要去做什么
	HitPayload TraceRay(const Ray& ray);//Insection Shader 被集成到这个里面了
	HitPayload ClosestHit(const Ray& ray,float HitDistance,uint32_t ObjectIndex);
	HitPayload Miss(const Ray& ray);
private:
	std::shared_ptr<Walnut::Image> m_FinalImage;

	Settings m_Settings;

	const Camera* m_ActiveCamera = nullptr;//当前是成员变量不是指针就是值，不可能是引用，想什么呢，指向活动相机的指针
	const Scene* m_ActiveScene = nullptr;//指向活动场景的指针 

	uint32_t* m_ImageData = nullptr;

	glm::vec4* m_AccumulationData = nullptr;//路径追踪中用来累计数据的指针数组

	uint32_t m_FrameIndex = 1;//当前帧数，要用来做除法里的除数

};