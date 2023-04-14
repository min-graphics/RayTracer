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
void Renderer::Render(const Camera& camera,const Scene& scene)
{
	m_ActiveCamera = &camera;
	m_ActiveScene = &scene;

	//const glm::vec3& rayOrigin = camera.GetPosition();

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
	
			PerPixel(x,y);//ÿ�����ض�Ҫ�����������

			//ray.direction = camera.GetRayDirections()[y * m_FinalImage->GetWidth() + x];	//����һ��ray cast,��������ᱻ�ƶ���PerPixel()����ȥ
			
			glm::vec4 color = PerPixel(x,y);//�õ�����׷�٣��õ�ÿһ�����ص���ɫ�����ظ�vec4 color����,�����ⲻһ������ȷ�ģ���Ϊ��ʱ��Ό��ݵ�ԭ�򣬽��ж��ز����������������ԭ�������Ӱ��shadow mapping����ԭ����Ҫ���ǽ����߻����������з�����
			color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));//���������clamp��0-1֮��
			m_ImageData[x + y * m_FinalImage->GetWidth()] = Utils::ConvertToRGBA(color);//������ת����RGBA��ʽ

			////m_ImageData[i] = 0xffff00ff;//�������Ҷ���ABGR�����������RGBA������С�����а�
			//m_ImageData[i] = Walnut::Random::UInt();//�����һ������
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
	//�������±�����Ҫ��ôȥѡ��,������API���У������»���ID���
	ray.direction = m_ActiveCamera->GetRayDirections()[y * m_FinalImage->GetWidth() + x];	//����һ��ray cast

	glm::vec3 color(0.0f);
	float multiplier = 1.0f;//˥��ϵ����һ��ʼû��˥������1,֮��С��1.��ӹ���ֻ��Ҫ˥�����ξͿ��ԡ�

	int bounces = 2;
	for (int i = 0; i < bounces; i++)
	{
		Renderer::HitPayload payload = TraceRay(ray);
		if (payload.HitDistance == -1)
		{
			glm::vec3 skyColor = glm::vec3(0.0f, 0.0f, 0.0f);
			color += skyColor * multiplier;
			break;//��break������ֱ��return��Ϊ�˲�����ѭ����breakֻ���˳���ǰѭ����

			//return glm::vec4(0, 0, 0, 1);
		}

		glm::vec3 lightDirection = glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f));
		float lightIntensitty = glm::max(glm::dot(payload.WorldNormal, -lightDirection), 0.0f);//��λ�����ĵ��������ֵ��һ����

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

//��ʵ��ÿһ������
Renderer::HitPayload Renderer::TraceRay(const Ray& ray)
{
	/*uint8_t r = (uint8_t)(coord.x * 255.f);
	uint8_t g = (uint8_t)(coord.y * 255.f);*/

	/*
	����ray�ˣ������������滻��
	glm::vec3 rayOrigin(0.0f,0.0f,1.0f);
	glm::vec3 rayDirection(coord.x, coord.y, -1.0f);
	*/

	/*
	����Ҫ����ˣ���Ϊ��һ��ѭ���ˣ�����˵���Բ�д���
	if (scene.Spheres.size() == 0)
	{
		return glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	}*/

	int closestSphere = -1;
	//uint32_t cloestSphere = UINT32_MAX;Ч����һ����

	//float hitDistance = FLT_MAX;
	float hitDistance = std::numeric_limits<float>::max();
	uint32_t index = 0;
	for (size_t i = 0; i < Renderer::m_ActiveScene->Spheres.size(); i++)
	{
		const Sphere& sphere = m_ActiveScene->Spheres[i];
		float radius = sphere.radius;
		//rayDirection = glm::normalize(rayDirection);//���й�һ���Ŀ����Ǿ޴�ġ�

		// (bx ^ 2 + by ^ 2)t ^ 2 + (2(axbx + ayby))t + (ax ^ 2 + ay ^ 2 - r ^ 2) = 0
		//	where
		//	a�ǹ������
		//	b�ǹ��������ķ���
		//	r�ǰ뾶
		//	t = hit distance,t�Ǵﵽ�Ĳ���

		glm::vec3 origin = ray.origin - sphere.position;//��ȥ�ƶ����壬����ȥ�ƶ�������������λ�ƣ���ʵ��һ���ġ�

		//float a = rayDirection.x * rayDirection.x + rayDirection.y * rayDirection.y + rayDirection.z* rayDirection.z;
		float a = glm::dot(ray.direction, ray.direction);//�����a��t^2��ϵ��
		float b = 2.0f * glm::dot(origin, ray.direction);
		float c = glm::dot(origin, origin) - radius * radius;

		//quadratic forumula discriminant
		//b^2-4ac

		float discriminant = b * b - 4.0f * a * c;
		if (discriminant < 0.0f)
		{
			//return glm::vec4(0, 0, 0, 1);//0xff000000;//��ѭ����Ͳ���return�ˣ�Ҫȥ����һ����continue
			continue;
		}

		//  (-b +- sqrt(discriminant))/2a
		//float t0 = (-b + glm::sqrt(discriminant)) / (2.0f * a);������ᱻ�õ�
		float closestT = (-b - glm::sqrt(discriminant)) / (2.0f * a);//��Сֵ
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

	glm::vec3 origin = ray.origin - closestSphere.position;//��ȥ�ƶ����壬����ȥ�ƶ�������������λ�ƣ���ʵ��һ���ġ�
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


