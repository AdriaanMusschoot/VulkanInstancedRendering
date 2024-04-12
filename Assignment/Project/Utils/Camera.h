#ifndef VK_CAMERA_H
#define VK_CAMERA_H
#include "Engine/Configuration.h"
#include <GLFW/glfw3.h>

namespace ave
{

	class Camera
	{
	public:
		Camera(GLFWwindow* windowPtr, const glm::vec3& origin, float fovAngle, float aspectRatio);
		~Camera() = default;

		void Update();
		const glm::mat4& GetViewMatrix() const;
		const glm::mat4& GetProjectionMatrix() const;
		const glm::vec3& GetCameraPosition() const;
	private:
		glm::vec3 m_Origin{};
		float m_FovAngle{ 90.f };
		float m_Fov{ tanf((m_FovAngle * ave::ToRadians) / 2.f) };
		float m_SpeedRotation{ 5.0f };
		float m_SpeedTranslation{ 5.0f };

		glm::vec3 m_Forward{ glm::vec3{ 0, 0, 1 } };
		glm::vec3 m_Up{ glm::vec3{ 0, -1, 0 } };
		glm::vec3 m_Right{ glm::vec3{ 1, 0, 0 } };

		float m_FarPlane{ 100.0f };
		float m_NearPlane{ 0.1f };
		float m_AspectRatio{};
		
		double m_TotalPitch{};
		double m_TotalYaw{};

		double m_LastMousePositionX{};
		double m_LastMousePositionY{};

		glm::mat4 m_InvViewMatrix{};
		glm::mat4 m_ViewMatrix{};
		glm::mat4 m_ProjectionMatrix{};
		
		GLFWwindow* m_WindowPtr{};

		void CalculateViewMatrix();
		void CalculateProjectionMatrix();
	};

}

#endif