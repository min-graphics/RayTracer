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
void Renderer::Render(const Camera& camera,const Scene& scene)
{
	m_ActiveCamera = &camera;
	m_ActiveScene = &scene;

	//const glm::vec3& rayOrigin = camera.GetPosition();

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
	
			PerPixel(x,y);//每个像素都要调用这个函数

			//ray.direction = camera.GetRayDirections()[y * m_FinalImage->GetWidth() + x];	//这是一个ray cast,但是这个会被移动到PerPixel()当中去
			
			glm::vec4 color = PerPixel(x,y);//得到光线追踪，得到每一个像素的颜色，返回给vec4 color变量,但是这不一定是正确的，因为有时候会抗锯齿的原因，进行多重采样。而且漫反射的原因或者阴影（shadow mapping）的原因，需要我们将射线击中物体后进行反弹。
			color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));//把这个变量clamp到0-1之间
			m_ImageData[x + y * m_FinalImage->GetWidth()] = Utils::ConvertToRGBA(color);//把像素转化成RGBA格式

			////m_ImageData[i] = 0xffff00ff;//从左往右读是ABGR，右往左读是RGBA，这是小端排列吧
			//m_ImageData[i] = Walnut::Random::UInt();//随机的一个整型
			//m_ImageData[i] |= 0xff000000;
		}
		//m_FinalImage->SetData(m_ImageData);
	}
	m_FinalImage->SetData(m_ImageData);
}

glm::vec4 Renderer::PerPixel(uint32_t x,uint32_t y)
{
	Ray ray;
	ray.origin = m_ActiveCamera->GetPosition();
	//问题是下标我们要怎么去选择,在其他API当中，是由下划线ID这个
	ray.direction = m_ActiveCamera->GetRayDirections()[y * m_FinalImage->GetWidth() + x];	//这是一个ray cast

	glm::vec3 color(0.0f);
	float multiplier = 1.0f;//衰减系数，一开始没有衰减，是1,之后小于1.间接光照只需要衰减两次就可以。

	int bounces = 2;
	for (int i = 0; i < bounces; i++)
	{
		Renderer::HitPayload payload = TraceRay(ray);
		if (payload.HitDistance == -1)
		{
			glm::vec3 skyColor = glm::vec3(0.0f, 0.0f, 0.0f);
			color += skyColor * multiplier;
			break;//用break而不是直接return是为了不结束循环，break只是退出当前循环。

			//return glm::vec4(0, 0, 0, 1);
		}

		glm::vec3 lightDirection = glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f));
		float lightIntensitty = glm::max(glm::dot(payload.WorldNormal, -lightDirection), 0.0f);//单位向量的点积与余弦值是一样的

		const Sphere& sphere = m_ActiveScene->Spheres[payload.ObjectIndex];
		glm::vec3 sphereColor = sphere.albedo;
		sphereColor *= lightIntensitty;
		color += sphereColor * multiplier;
			
		multiplier *= 0.7f;

		ray.origin = payload.WorldPosition+payload.WorldNormal*0.0001f;
		ray.direction = glm::reflect(ray.direction, payload.WorldNormal);
	}

	return glm::vec4(color, 1.0f);
	//return glm::vec4(normal,1.0f);
}

//其实是每一条光线
Renderer::HitPayload Renderer::TraceRay(const Ray& ray)
{
	/*uint8_t r = (uint8_t)(coord.x * 255.f);
	uint8_t g = (uint8_t)(coord.y * 255.f);*/

	/*
	有了ray了，这两个可以替换了
	glm::vec3 rayOrigin(0.0f,0.0f,1.0f);
	glm::vec3 rayDirection(coord.x, coord.y, -1.0f);
	*/

	/*
	不需要这个了，因为有一个循环了，所以说可以不写这个
	if (scene.Spheres.size() == 0)
	{
		return glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	}*/

	int closestSphere = -1;
	//uint32_t cloestSphere = UINT32_MAX;效果是一样的

	//float hitDistance = FLT_MAX;
	float hitDistance = std::numeric_limits<float>::max();
	uint32_t index = 0;
	for (size_t i = 0; i < Renderer::m_ActiveScene->Spheres.size(); i++)
	{
		const Sphere& sphere = m_ActiveScene->Spheres[i];
		float radius = sphere.radius;
		//rayDirection = glm::normalize(rayDirection);//进行归一化的开销是巨大的。

		// (bx ^ 2 + by ^ 2)t ^ 2 + (2(axbx + ayby))t + (ax ^ 2 + ay ^ 2 - r ^ 2) = 0
		//	where
		//	a是光线起点
		//	b是光线向量的方向
		//	r是半径
		//	t = hit distance,t是达到的参数

		glm::vec3 origin = ray.origin - sphere.position;//不去移动物体，而是去移动相机，根据相对位移，其实是一样的。

		//float a = rayDirection.x * rayDirection.x + rayDirection.y * rayDirection.y + rayDirection.z* rayDirection.z;
		float a = glm::dot(ray.direction, ray.direction);//这里的a是t^2的系数
		float b = 2.0f * glm::dot(origin, ray.direction);
		float c = glm::dot(origin, origin) - radius * radius;

		//quadratic forumula discriminant
		//b^2-4ac

		float discriminant = b * b - 4.0f * a * c;
		if (discriminant < 0.0f)
		{
			//return glm::vec4(0, 0, 0, 1);//0xff000000;//再循环里就不是return了，要去找下一个，continue
			continue;
		}

		//  (-b +- sqrt(discriminant))/2a
		//float t0 = (-b + glm::sqrt(discriminant)) / (2.0f * a);这个不会被用到
		float closestT = (-b - glm::sqrt(discriminant)) / (2.0f * a);//较小值
		if (closestT>0.0f && closestT < hitDistance)
		{
			hitDistance = closestT;
			closestSphere = i;
		}
	}

	if (closestSphere == -1)
	{
		return Miss(ray);
	}
	return ClosestHit(ray, hitDistance, closestSphere);

}
Renderer::HitPayload Renderer::ClosestHit(const Ray& ray, float HitDistance, uint32_t ObjectIndex)
{

	Renderer::HitPayload payLoad;
	payLoad.HitDistance = HitDistance;
	payLoad.ObjectIndex = ObjectIndex;

	//const Sphere& sphere = scene.Spheres[0];

	//glm::vec3 h0 = rayOrigin + rayDirection * t0;

	const Sphere& closestSphere = m_ActiveScene->Spheres[ObjectIndex];

	glm::vec3 origin = ray.origin - closestSphere.position;//不去移动物体，而是去移动相机，根据相对位移，其实是一样的。
	payLoad.WorldPosition = origin + ray.direction * HitDistance;
	payLoad.WorldNormal = glm::normalize(payLoad.WorldPosition);

	payLoad.WorldPosition += closestSphere.position;

	return payLoad;
}
Renderer::HitPayload Renderer::Miss(const Ray& ray)
{
	Renderer::HitPayload payLoad;
	payLoad.HitDistance = -1.0f;
	return payLoad;
}


