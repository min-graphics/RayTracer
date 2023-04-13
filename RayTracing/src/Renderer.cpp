#include"Renderer.h"

#include"Walnut/Random.h"

namespace Utils
{
	static uint32_t ConvertToRGBA(const glm::vec4& color)
	{
		uint8_t r = (uint8_t)(color.r * 255.f);//加不加括号很重要。。。。
		uint8_t g = (uint8_t)(color.g * 255.f);
		uint8_t b = (uint8_t)(color.b * 255.f);
		uint8_t a = (uint8_t)(color.a * 255.f);

		uint32_t result = (a << 24) | (b << 16) | (g << 8) | (r);//这里是或，不是与
		return result;
	}
}

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
void Renderer::Render(const Camera& camera)
{
	//const glm::vec3& rayOrigin = camera.GetPosition();

	Ray ray;
	ray.origin = camera.GetPosition();


	for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++)//最外层是y，是因为我们希望对cpu缓存更加的友好
	{
		//render every pixel
		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++)
		{
			//不需要这些了，因为下面已经做了计算了光线方向了
			//glm::vec2 coord = { (float)x / (float)m_FinalImage->GetWidth(),(float)y / (float)m_FinalImage->GetHeight() };//float是防止两个整数相除得到0;
			//coord = coord * 2.0f - 1.0f; // -1 -> 1

			//应该是per pixel才对
			//const glm::vec3& rayDirection = camera.GetRayDirections()[y*m_FinalImage->GetWidth()+x];

			ray.direction = camera.GetRayDirections()[y * m_FinalImage->GetWidth() + x];

			glm::vec4 color = TraceRay(ray);
			color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));
			m_ImageData[x + y * m_FinalImage->GetWidth()] = Utils::ConvertToRGBA(color);

			////m_ImageData[i] = 0xffff00ff;//从左往右读是ABGR，右往左读是RGBA，这是小端排列吧
			//m_ImageData[i] = Walnut::Random::UInt();//随机的一个整型
			//m_ImageData[i] |= 0xff000000;
		}
		//m_FinalImage->SetData(m_ImageData);
	}
	m_FinalImage->SetData(m_ImageData);
}
//其实是每一条光线
glm::vec4 Renderer::TraceRay(const Ray& ray)
{
	/*uint8_t r = (uint8_t)(coord.x * 255.f);
	uint8_t g = (uint8_t)(coord.y * 255.f);*/

	/*
	有了ray了，这两个可以替换了
	glm::vec3 rayOrigin(0.0f,0.0f,1.0f);
	glm::vec3 rayDirection(coord.x, coord.y, -1.0f);
	*/

	float radius = 0.5f;
	//rayDirection = glm::normalize(rayDirection);//进行归一化的开销是巨大的。

	// (bx ^ 2 + by ^ 2)t ^ 2 + (2(axbx + ayby))t + (ax ^ 2 + ay ^ 2 - r ^ 2) = 0
	//	where
	//	a是光线起点
	//	b是光线向量的方向
	//	r是半径
	//	t = hit distance,t是达到的参数

	//float a = rayDirection.x * rayDirection.x + rayDirection.y * rayDirection.y + rayDirection.z* rayDirection.z;
	float a = glm::dot(ray.direction, ray.direction);//这里的a是t^2的系数
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
	float cloestT = (-b - glm::sqrt(discriminant)) / (2.0f * a);//较小值

	//glm::vec3 h0 = rayOrigin + rayDirection * t0;

	glm::vec3 hitPoint = ray.origin + ray.direction * cloestT;
	glm::vec3 normal = glm::normalize(hitPoint);

	glm::vec3 lightDirection = glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f));

	float d = glm::max(glm::dot(normal, -lightDirection),0.0f);//单位向量的点积与余弦值是一样的

	glm::vec3 sphereColor(1, 0, 1);
	sphereColor *= d;

	return glm::vec4(sphereColor, 1.0f);
	//return glm::vec4(normal,1.0f);
}
