#include "atpch.h"
#include "OrthographicCamera.h"
#include "Atlas/Core/Application.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Atlas {

static glm::mat4 glToMetal = glm::mat4(
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 0.5f, 0.0f,
	0.0f, 0.0f, 0.5f, 1.0f);

constexpr float nearPlane = -100.0f;
constexpr float farPlane = 0.0f;

OrthographicCamera::OrthographicCamera(float left, float right, float top, float bottom) : m_projectionMatrix(glm::ortho(left, right, bottom, top, nearPlane, farPlane)), m_viewMatrix(1.0f) {
	AT_PROFILE_FUNCTION();

	m_projectionMatrix = glToMetal * m_projectionMatrix;
	m_viewProjectionMatrix = m_projectionMatrix * m_viewMatrix;

	m_viewportHeight = Application::get().getWindow().getHeight();
}

void OrthographicCamera::setProjection(float left, float right, float top, float bottom) {
	AT_PROFILE_FUNCTION();

	m_projectionMatrix = glToMetal * glm::ortho(left, right, bottom, top, nearPlane, farPlane);
	m_viewProjectionMatrix = m_projectionMatrix * m_viewMatrix;
}

void OrthographicCamera::recalculateViewMatrix() {
	AT_PROFILE_FUNCTION();

	glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_position) *
						  glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation), glm::vec3(0, 0, 1));

	m_viewMatrix = glm::inverse(transform);
	m_viewProjectionMatrix = m_projectionMatrix * m_viewMatrix;
}

}  // namespace Atlas