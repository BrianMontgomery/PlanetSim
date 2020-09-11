#include "PSIMPCH.h"
#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

#include "Application/Application.h"

Camera::Camera(float fov)
	: m_ViewMatrix(1.0f)
{
	PSIM_PROFILE_FUNCTION();

	setProjection(fov);
}

void Camera::setProjection(float fov)
{
	PSIM_PROFILE_FUNCTION();

	Application& app = Application::Get();

	m_ProjectionMatrix = glm::perspective(glm::radians(fov), (float)app.GetWindow().GetWidth() / (float)app.GetWindow().GetHeight(), 0.1f, 100.0f);
	recalculateViewProjectionMatrix();
}

void Camera::recalculateViewMatrix()
{
	PSIM_PROFILE_FUNCTION();

	m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Front, m_Up);
	m_ViewMatrix = glm::rotate(m_ViewMatrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	recalculateViewProjectionMatrix();
}

void Camera::recalculateViewProjectionMatrix() {
	m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}