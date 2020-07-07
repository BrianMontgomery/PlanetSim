#pragma once

#include <glm/glm.hpp>

class Camera
{
public:
	Camera(float fov);

	void setProjection(float fov);

	const glm::vec3& getPosition() const { return m_Position; }
	void setPosition(const glm::vec3& position) { m_Position = position; recalculateViewMatrix(); }
	void setUp(const glm::vec3& up) { m_Up = up; recalculateViewMatrix(); }
	void setFront(const glm::vec3& front) { m_Front = front; recalculateViewMatrix(); }

	float GetRotation() const { return m_Rotation; }
	void SetRotation(float rotation) { m_Rotation = rotation; recalculateViewMatrix(); }

	const glm::mat4& getProjectionMatrix() const { return m_ProjectionMatrix; }
	const glm::mat4& getViewMatrix() const { return m_ViewMatrix; }
	const glm::mat4& getViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

private:
	void recalculateViewMatrix();
	void recalculateViewProjectionMatrix();

private:
	glm::mat4 m_ProjectionMatrix;
	glm::mat4 m_ViewMatrix;
	glm::mat4 m_ViewProjectionMatrix;

	glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
	glm::vec3 m_Up = { 0.0f, 0.0f, 0.0f };
	glm::vec3 m_Front = { 0.0f, 0.0f, 0.0f };
	float m_Rotation = 0.0f;
};

