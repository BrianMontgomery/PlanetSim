#pragma once
#include "PSIM/Graphics/Camera/Camera.h"
#include "PSIM/Core/Debug/TimeStep.h"

#include "PSIM/Core/Events/ApplicationEvents.h"
#include "PSIM/Core/Events/MouseEvents.h"

class CameraController 
{
public:
	CameraController(float aspectRatio, bool rotation = false);

	void OnUpdate(Timestep ts);
	void OnEvent(Event& e);

	void OnResize(float width, float height);

	Camera& GetCamera() { return m_Camera; }
	const Camera& GetCamera() const { return m_Camera; }

	float GetZoomLevel() const { return m_ZoomLevel; }
	void SetZoomLevel(float level) { m_ZoomLevel = level; }

private:
	bool OnMouseScrolled(MouseScrolledEvent& e);
	bool OnWindowResized(WindowResizeEvent& e);
	bool OnMouseMoved(MouseMovedEvent& e);

private:
	Camera m_Camera;

	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	bool m_Rotation;
	float m_CameraRotation = 0.0f; //In degrees, in the anti-clockwise direction
	float m_CameraTranslationSpeed = 5.0f, m_CameraRotationSpeed = 180.0f;

	float m_AspectRatio;
	float m_ZoomLevel = 1.0f;

	bool firstMouse = true;
	float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
	float pitch = 0.0f;
	float lastX = 800.0f / 2.0;
	float lastY = 600.0 / 2.0;
	float fov = 45.0f;
};

