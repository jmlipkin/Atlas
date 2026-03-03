#pragma once

#include <glm/glm.hpp>

namespace Atlas {

class OrthographicCamera {
  public:
	OrthographicCamera(float left, float right, float top, float bottom);
	void setProjection(float left, float right, float top, float bottom);

	const glm::vec3& getPosition() const { return m_position; }
	void setPosition(const glm::vec3& position) {
		m_position = position;
		recalculateViewMatrix();
	}

    void setViewportHeight(float height) { m_viewportHeight = height; }

	float getRotation() const { return m_rotation; }
	void setRotation(float rotation) {
		m_rotation = rotation;
		recalculateViewMatrix();
	}

	float getZoom() const { return 1.0f / m_projectionMatrix[1][1]; }
	float getPixelsPerWorldUnit() const { return m_viewportHeight / std::abs(2.0f / m_projectionMatrix[1][1]); }

	const glm::mat4& getProjectionMatrix() const { return m_projectionMatrix; }
	const glm::mat4& getViewMatrix() const { return m_viewMatrix; }
	const glm::mat4& getViewProjectionMatrix() const { return m_viewProjectionMatrix; }

  private:
	void recalculateViewMatrix();

  private:
	glm::mat4 m_projectionMatrix;
	glm::mat4 m_viewMatrix;
	glm::mat4 m_viewProjectionMatrix;
	float m_viewportHeight;

	glm::vec3 m_position{0.0f, 0.0f, 0.0f};
	float m_rotation = 0.0f;
};

}  // namespace Atlas