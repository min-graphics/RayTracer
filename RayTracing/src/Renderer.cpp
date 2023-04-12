#include"Renderer.h"

void Renderer::OnResize(uint32_t width, uint32_t height)
{
	if (m_FinalImage)
	{
		//����Ҫ������С����ֱ������
		if (m_FinalImage->GetWidth() == width && m_FinalImage->GetHeight() == height)
		{
			return;
		}
		//��Ҫ�Ļ��͵��õ���ͼƬ��С����
		m_FinalImage->Resize(width, height);
	}
	else
	{	
		//û��ͼ����ȴ���һ��ͼ��һ������ָ��ָ���ͼ��
		m_FinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
	}

	//delete���������ͻ�����飬���Ը�������Ҫ���ǽ���if�п�
	delete[] m_ImageData;
	
	m_ImageData = new uint32_t[width * height];
}
void Renderer::Render()
{
	for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++)//�������y������Ϊ����ϣ����cpu������ӵ��Ѻ�
	{
		//render every pixel
		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++)
		{
			glm::vec2 coord = { (float)x / (float)m_FinalImage->GetWidth(),(float)y / (float)m_FinalImage->GetHeight() };//float�Ƿ�ֹ������������õ�0;
			coord = coord * 2.0f - 1.0f; // -1 -> 1
			m_ImageData[x + y * m_FinalImage->GetWidth()] = PerPixel(coord);

			////m_ImageData[i] = 0xffff00ff;//�������Ҷ���ABGR�����������RGBA������С�����а�
			//m_ImageData[i] = Walnut::Random::UInt();//�����һ������
			//m_ImageData[i] |= 0xff000000;
		}
		//m_FinalImage->SetData(m_ImageData);
	}
	m_FinalImage->SetData(m_ImageData);
}
//��ʵ��ÿһ������
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
	//	a�ǹ������
	//	b�ǹ��������ķ���
	//	r�ǰ뾶
	//	t = hit distance,t�Ǵﵽ�Ĳ���

	//float a = rayDirection.x * rayDirection.x + rayDirection.y * rayDirection.y + rayDirection.z* rayDirection.z;
	float a = glm::dot(rayDirection, rayDirection);//�����a��t^2��ϵ��
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
