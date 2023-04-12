#pragma once
#include"Walnut/Image.h"
#include<glm/glm.hpp>
#include<memory>

//输入是场景描述，输出是一个2D的图像，保存渲染器生成的像素。
class Renderer
{
public:
	Renderer() = default;

	void OnResize(uint32_t width, uint32_t height);
	void Render();

	std::shared_ptr<Walnut::Image> GetFinalImage()const
	{
		return m_FinalImage;
	}
private:
	//输入一个坐标，返回一个颜色.
	uint32_t PerPixel(glm::vec2 coord);

private:
	std::shared_ptr<Walnut::Image> m_FinalImage;
	uint32_t* m_ImageData = nullptr;
};