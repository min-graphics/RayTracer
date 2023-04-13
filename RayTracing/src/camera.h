#pragma once

#include <glm/glm.hpp>
#include <vector>

/*
虚幻风格相机，按住右键可以旋转相机
有点像第一人称相机,可以按住WASD飞来飞去
*/
class Camera
{
public:
	Camera(float verticalFOV, float nearClip, float farClip);//垂直视角，近平面和远平面三个参数来进行构造

	bool OnUpdate(float ts);//参数是每一帧使用的时间，利用这个来进行更新
	void OnResize(uint32_t width, uint32_t height);//调整大小，必须要重新计算投影矩阵

	const glm::mat4& GetProjection() const { return m_Projection; }
	const glm::mat4& GetInverseProjection() const { return m_InverseProjection; }
	const glm::mat4& GetView() const { return m_View; }
	const glm::mat4& GetInverseView() const { return m_InverseView; }

	const glm::vec3& GetPosition() const { return m_Position; }
	const glm::vec3& GetDirection() const { return m_ForwardDirection; }

	const std::vector<glm::vec3>& GetRayDirections() const { return m_RayDirections; }//为了提高实时性，缓存了一个射线方向

	float GetRotationSpeed();
private:
	void RecalculateProjection();
	void RecalculateView();
	void RecalculateRayDirections();
private:
	glm::mat4 m_Projection{ 1.0f };
	glm::mat4 m_View{ 1.0f };
	glm::mat4 m_InverseProjection{ 1.0f };
	glm::mat4 m_InverseView{ 1.0f };

	float m_VerticalFOV = 45.0f;
	float m_NearClip = 0.1f;
	float m_FarClip = 100.0f;

	glm::vec3 m_Position{ 0.0f, 0.0f, 0.0f };
	glm::vec3 m_ForwardDirection{ 0.0f, 0.0f, 0.0f };

	// Cached ray directions
	std::vector<glm::vec3> m_RayDirections;

	glm::vec2 m_LastMousePosition{ 0.0f, 0.0f };

	uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
};
