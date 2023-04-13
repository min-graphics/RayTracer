#pragma once
#include"Walnut/Image.h"
#include"camera.h"

#include"Ray.h"

#include<glm/glm.hpp>
#include<memory>

//�����ǳ��������������һ��2D��ͼ�񣬱�����Ⱦ�����ɵ����ء�
class Renderer
{
public:
	Renderer() = default;

	void OnResize(uint32_t width, uint32_t height);
	void Render(const Camera& m_camera);

	std::shared_ptr<Walnut::Image> GetFinalImage()const
	{
		return m_FinalImage;
	}
private:
	//����һ�����꣬����һ����ɫ.
	glm::vec4 TraceRay(const Ray& ray);

private:
	std::shared_ptr<Walnut::Image> m_FinalImage;
	uint32_t* m_ImageData = nullptr;
};