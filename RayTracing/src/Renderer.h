#pragma once
#include"Walnut/Image.h"
#include"camera.h"

#include"Ray.h"
#include"scene.h"

#include<glm/glm.hpp>
#include<memory>

//�����ǳ��������������һ��2D��ͼ�񣬱�����Ⱦ�����ɵ����ء�
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

	glm::vec4 PerPixel(uint32_t x,uint32_t y);//rayGen shader,��DX12����vulkan�У���ÿ��������Ⱦ�����ص��ã������ڸ���ɫ���У������Ƿ�ҪǱ�ڵ�ÿ������Ͷ���������

	////����һ�����꣬����һ����ɫ.
	//glm::vec4 TraceRay(const Ray& ray,const Scene& scene);

	//�µ�TraceRay��������Ӧ�������ߣ�����ǻ����������û�л���������غ�Payload,Ҳ����������Ҫȥ��ʲô
	HitPayload TraceRay(const Ray& ray);//Insection Shader �����ɵ����������
	HitPayload ClosestHit(const Ray& ray,float HitDistance,uint32_t ObjectIndex);
	HitPayload Miss(const Ray& ray);
private:
	std::shared_ptr<Walnut::Image> m_FinalImage;

	Settings m_Settings;

	const Camera* m_ActiveCamera = nullptr;//��ǰ�ǳ�Ա��������ָ�����ֵ�������������ã���ʲô�أ�ָ�������ָ��
	const Scene* m_ActiveScene = nullptr;//ָ��������ָ�� 

	uint32_t* m_ImageData = nullptr;

	glm::vec4* m_AccumulationData = nullptr;//·��׷���������ۼ����ݵ�ָ������

	uint32_t m_FrameIndex = 1;//��ǰ֡����Ҫ������������ĳ���

};