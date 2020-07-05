#include "PSIMPCH.h"
#include "CameraController.h"

#include "Core/Input/Input.h"
#include "Core/Input/KeyCodes.h"

CameraController::CameraController(float aspectRatio, bool rotation)
	: m_AspectRatio(aspectRatio), m_Camera(fov), m_Rotation(rotation)
{
}

void CameraController::OnUpdate(Timestep ts)
{
	PSIM_PROFILE_FUNCTION();

	if (Input::IsKeyPressed(PSIM_KEY_A))
	{
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * (ts * m_CameraTranslationSpeed);
	}
	else if (Input::IsKeyPressed(PSIM_KEY_D))
	{
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * (ts * m_CameraTranslationSpeed);
	}

	if (Input::IsKeyPressed(PSIM_KEY_W))
	{
		cameraPos += cameraFront * (m_CameraTranslationSpeed * ts);
	}
	else if (Input::IsKeyPressed(PSIM_KEY_S))
	{
		cameraPos -= cameraFront * (m_CameraTranslationSpeed * ts);
	}

	m_Camera.setPosition(cameraPos);

	m_CameraTranslationSpeed = m_ZoomLevel;
}

void CameraController::OnEvent(Event& e)
{
	PSIM_PROFILE_FUNCTION();

	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<MouseScrolledEvent>(PSIM_BIND_EVENT_FN(CameraController::OnMouseScrolled));
	dispatcher.Dispatch<MouseMovedEvent>(PSIM_BIND_EVENT_FN(CameraController::OnMouseMoved));
	dispatcher.Dispatch<WindowResizeEvent>(PSIM_BIND_EVENT_FN(CameraController::OnWindowResized));
}

void CameraController::OnResize(float width, float height)
{
	m_AspectRatio = width / height;
	m_Camera.setProjection(fov);
}

bool CameraController::OnMouseScrolled(MouseScrolledEvent& e)
{
	PSIM_PROFILE_FUNCTION();

	m_ZoomLevel -= e.GetYOffset() * 0.25f;
	m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
	m_Camera.setProjection(fov);
	return false;
}

bool CameraController::OnMouseMoved(MouseMovedEvent& e)
{
	PSIM_PROFILE_FUNCTION();

	if (firstMouse)
	{
		lastX = e.GetX();
		lastY = e.GetY();
		firstMouse = false;
	}

	float xoffset = e.GetX() - lastX;
	float yoffset = lastY - e.GetY(); // reversed since y-coordinates go from bottom to top
	lastX = e.GetX();
	lastY = e.GetY();

	float sensitivity = 0.1f; // change this value to your liking
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);

	return false;
}

bool CameraController::OnWindowResized(WindowResizeEvent& e)
{
	PSIM_PROFILE_FUNCTION();

	OnResize((float)e.GetWidth(), (float)e.GetHeight());
	return false;
}