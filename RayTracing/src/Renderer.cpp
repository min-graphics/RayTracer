#include"Renderer.h"

void Renderer::OnResize(uint32_t width, uint32_t height)
{
	if (m_FinalImage)
	{
		//不需要调整大小，就直接跳出
		if (m_FinalImage->GetWidth() == width && m_FinalImage->GetHeight() == height)
		{
			return;
		}
		//需要的话就调用调整图片大小函数
		m_FinalImage->Resize(width, height);
	}
	else
	{	
		//没有图像就先创建一个图像。一个智能指针指向该图像
		m_FinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
	}

	//delete运算符本身就会做检查，所以根本不需要我们进行if判空
	delete[] m_ImageData;
	
	m_ImageData = new uint32_t[width * height];
}
void Renderer::Render()
{
	for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++)//最外层是y，是因为我们希望对cpu缓存更加的友好
	{
		//render every pixel
		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++)
		{
			glm::vec2 coord = { (float)x / (float)m_FinalImage->GetWidth(),(float)y / (float)m_FinalImage->GetHeight() };//float是防止两个整数相除得到0;
			coord = coord * 2.0f - 1.0f; // -1 -> 1
			m_ImageData[x + y * m_FinalImage->GetWidth()] = PerPixel(coord);

			////m_ImageData[i] = 0xffff00ff;//从左往右读是ABGR，右往左读是RGBA，这是小端排列吧
			//m_ImageData[i] = Walnut::Random::UInt();//随机的一个整型
			//m_ImageData[i] |= 0xff000000;
		}
		//m_FinalImage->SetData(m_ImageData);
	}
	m_FinalImage->SetData(m_ImageData);
}
//其实是每一条光线
uint32_t Renderer::PerPixel(glm::vec2 coord)
{
	uint8_t r = (uint8_t)(coord.x * 255.f);
	uint8_t g = (uint8_t)(coord.y * 255.f);

	glm::vec3 rayOrigin(0.0f,0.0f,2.0f);
	glm::vec3 rayDirection(coord.x, coord.y, -1.0f);
	float radius = 0.5f;
	//rayDirection = glm::normalize(rayDirection);

	// (bx ^ 2 + by ^ 2)t ^ 2 + (2(axbx + ayby))t + (ax ^ 2 + ay ^ 2 - r ^ 2) = 0
	//	where
	//	a是光线起点
	//	b是光线向量的方向
	//	r是半径
	//	t = hit distance,t是达到的参数

	//float a = rayDirection.x * rayDirection.x + rayDirection.y * rayDirection.y + rayDirection.z* rayDirection.z;
	float a = glm::dot(rayDirection, rayDirection);//这里的a是t^2的系数
	float b = 2.0f * glm::dot(rayOrigin,rayDirection);
	float c = glm::dot(rayOrigin, rayOrigin) - radius * radius;

	//quadratic forumula discriminant
	//b^2-4ac
	float discriminant = b * b - 4.0f * a * c;
	if (discriminant >= 0.0f)
	{
		return 0xffff00ff;
	}
	return 0xff000000;
}
