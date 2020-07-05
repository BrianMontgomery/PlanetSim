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

	glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position) *
		glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation), glm::vec3(0, 0, 1));

	m_ViewMatrix = glm::inverse(transform);
	recalculateViewProjectionMatrix();
}

void Camera::recalculateViewProjectionMatrix() {
	m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}