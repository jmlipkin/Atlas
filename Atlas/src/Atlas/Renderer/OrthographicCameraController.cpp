#include "OrthographicCameraController.h"
#include "atpch.h"

#include "Atlas/Core/Input.h"
#include "Atlas/Core/KeyCodes.h"

namespace Atlas {
    OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool enableRotation) : m_aspectRatio(aspectRatio), m_camera(-m_aspectRatio * m_zoom, m_aspectRatio * m_zoom, -m_zoom, m_zoom), m_rotationEnabled(enableRotation) {
        m_cameraPosition = glm::vec3(0.0f);
		m_rotation = 0.0f;
    }

    void OrthographicCameraController::onUpdate(DeltaTime dt) {
        AT_PROFILE_FUNCTION();
        
        if(Input::isKeyPressed(AT_KEY_A)) {
            m_cameraPosition.x -= cos(glm::radians(m_rotation)) * m_translateSpeed * dt;
            m_cameraPosition.y -= sin(glm::radians(m_rotation)) * m_translateSpeed * dt;
        }
        if(Input::isKeyPressed(AT_KEY_D)) {
            m_cameraPosition.x += cos(glm::radians(m_rotation)) * m_translateSpeed * dt;
            m_cameraPosition.y += sin(glm::radians(m_rotation)) * m_translateSpeed * dt;
        }
        if(Input::isKeyPressed(AT_KEY_W)) {
            m_cameraPosition.x -= -sin(glm::radians(m_rotation)) * m_translateSpeed * dt;
            m_cameraPosition.y -= cos(glm::radians(m_rotation)) * m_translateSpeed * dt;
        }
        if(Input::isKeyPressed(AT_KEY_S)) {
            m_cameraPosition.x += -sin(glm::radians(m_rotation)) * m_translateSpeed * dt;
            m_cameraPosition.y += cos(glm::radians(m_rotation)) * m_translateSpeed * dt;
        }

        if(m_rotationEnabled) {
            if(Input::isKeyPressed(AT_KEY_Q)) {
                m_rotation += m_rotationSpeed * dt;
            }
            if(Input::isKeyPressed(AT_KEY_E)) {
                m_rotation -= m_rotationSpeed * dt;
            }

            if (m_rotation > 180.0f)
                m_rotation -= 360.0f;
            else if(m_rotation <= -180.0f)
                m_rotation += 360.0f;

            m_camera.setRotation(m_rotation);
        }
        m_camera.setPosition(m_cameraPosition);
        
        m_translateSpeed = m_zoom;
	}

	void OrthographicCameraController::onResize(float width, float height) {
		m_aspectRatio = width / height;
        m_camera.setProjection(-m_aspectRatio * m_zoom, m_aspectRatio * m_zoom, -m_zoom, m_zoom);
    }

    void OrthographicCameraController::onEvent(Event& event) {
        EventDispatcher dispatcher(event);
        dispatcher.dispatch<MouseScrolledEvent>(AT_BIND_EVENT_FN(OrthographicCameraController::onMouseScrolled));
        dispatcher.dispatch<WindowResizeEvent>(AT_BIND_EVENT_FN(OrthographicCameraController::onWindowResize));
    }

    bool OrthographicCameraController::onMouseScrolled(MouseScrolledEvent& e) {
        m_zoom -= e.getYOffset() * 0.25f;
        m_zoom = std::max(m_zoom, 0.25f);
        // m_camera.setProjection(-m_aspectRatio * m_zoom, m_aspectRatio * m_zoom, -m_zoom, m_zoom);
    
        return false;
    }

    bool OrthographicCameraController::onWindowResize(WindowResizeEvent& e) {
		m_aspectRatio = (float)e.getWidth() / (float)e.getHeight();
        m_camera.setViewportHeight(e.getHeight());
        m_camera.setProjection(-m_aspectRatio * m_zoom, m_aspectRatio * m_zoom, -m_zoom, m_zoom);
        return false;
    }
}