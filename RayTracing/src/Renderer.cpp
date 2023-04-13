#include"Renderer.h"

#include"Walnut/Random.h"

namespace Utils
{
	static uint32_t ConvertToRGBA(const glm::vec4& color)
	{
		uint8_t r = (uint8_t)(color.r * 255.f);//�Ӳ������ź���Ҫ��������
		uint8_t g = (uint8_t)(color.g * 255.f);
		uint8_t b = (uint8_t)(color.b * 255.f);
		uint8_t a = (uint8_t)(color.a * 255.f);

		uint32_t result = (a << 24) | (b << 16) | (g << 8) | (r);//�����ǻ򣬲�����
		return result;
	}
}

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
void Renderer::Render(const Camera& camera)
{
	//const glm::vec3& rayOrigin = camera.GetPosition();

	Ray ray;
	ray.origin = camera.GetPosition();


	for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++)//�������y������Ϊ����ϣ����cpu������ӵ��Ѻ�
	{
		//render every pixel
		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++)
		{
			//����Ҫ��Щ�ˣ���Ϊ�����Ѿ����˼����˹��߷�����
			//glm::vec2 coord = { (float)x / (float)m_FinalImage->GetWidth(),(float)y / (float)m_FinalImage->GetHeight() };//float�Ƿ�ֹ������������õ�0;
			//coord = coord * 2.0f - 1.0f; // -1 -> 1

			//Ӧ����per pixel�Ŷ�
			//const glm::vec3& rayDirection = camera.GetRayDirections()[y*m_FinalImage->GetWidth()+x];

			ray.direction = camera.GetRayDirections()[y * m_FinalImage->GetWidth() + x];

			glm::vec4 color = TraceRay(ray);
			color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));
			m_ImageData[x + y * m_FinalImage->GetWidth()] = Utils::ConvertToRGBA(color);

			////m_ImageData[i] = 0xffff00ff;//�������Ҷ���ABGR�����������RGBA������С�����а�
			//m_ImageData[i] = Walnut::Random::UInt();//�����һ������
			//m_ImageData[i] |= 0xff000000;
		}
		//m_FinalImage->SetData(m_ImageData);
	}
	m_FinalImage->SetData(m_ImageData);
}
//��ʵ��ÿһ������
glm::vec4 Renderer::TraceRay(const Ray& ray)
{
	/*uint8_t r = (uint8_t)(coord.x * 255.f);
	uint8_t g = (uint8_t)(coord.y * 255.f);*/

	/*
	����ray�ˣ������������滻��
	glm::vec3 rayOrigin(0.0f,0.0f,1.0f);
	glm::vec3 rayDirection(coord.x, coord.y, -1.0f);
	*/

	float radius = 0.5f;
	//rayDirection = glm::normalize(rayDirection);//���й�һ���Ŀ����Ǿ޴�ġ�

	// (bx ^ 2 + by ^ 2)t ^ 2 + (2(axbx + ayby))t + (ax ^ 2 + ay ^ 2 - r ^ 2) = 0
	//	where
	//	a�ǹ������
	//	b�ǹ��������ķ���
	//	r�ǰ뾶
	//	t = hit distance,t�Ǵﵽ�Ĳ���

	//float a = rayDirection.x * rayDirection.x + rayDirection.y * rayDirection.y + rayDirection.z* rayDirection.z;
	float a = glm::dot(ray.direction, ray.direction);//�����a��t^2��ϵ��
	float b = 2.0f * glm::dot(ray.origin,ray.direction);
	float c = glm::dot(ray.origin, ray.origin) - radius * radius;

	//quadratic forumula discriminant
	//b^2-4ac

	float discriminant = b * b - 4.0f * a * c;
	if (discriminant < 0.0f)
	{
		return glm::vec4(0, 0, 0, 1);//0xff000000;
	}

	//  (-b +- sqrt(discriminant))/2a
	float t0 = (-b + glm::sqrt(discriminant)) / (2.0f * a);
	float cloestT = (-b - glm::sqrt(discriminant)) / (2.0f * a);//��Сֵ

	//glm::vec3 h0 = rayOrigin + rayDirection * t0;

	glm::vec3 hitPoint = ray.origin + ray.direction * cloestT;
	glm::vec3 normal = glm::normalize(hitPoint);

	glm::vec3 lightDirection = glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f));

	float d = glm::max(glm::dot(normal, -lightDirection),0.0f);//��λ�����ĵ��������ֵ��һ����

	glm::vec3 sphereColor(1, 0, 1);
	sphereColor *= d;

	return glm::vec4(sphereColor, 1.0f);
	//return glm::vec4(normal,1.0f);
}
