#pragma once

#include <glm/glm.hpp>

#include "Atlas/Core/Time.h"
#include "Atlas/Events/ApplicationEvent.h"
#include "Atlas/Events/MouseEvent.h"
#include "OrthographicCamera.h"

namespace Atlas {
    
class OrthographicCameraController {
   public:
    OrthographicCameraController(float aspectRatio, bool enableRotation = false);
    void onUpdate(DeltaTime dt);
    void onEvent(Event& e);

    OrthographicCamera& getCamera() { return m_camera; }
    const OrthographicCamera& getCamera() const { return m_camera; }

    float getZoomLevel() const { return m_zoom; }
    void setZoomLevel(float zoom) { m_zoom = zoom;
        m_camera.setProjection(-m_aspectRatio * m_zoom, m_aspectRatio * m_zoom, -m_zoom, m_zoom);
    }

   private:
    bool onMouseScrolled(MouseScrolledEvent& e);
    bool onWindowResize(WindowResizeEvent& e);

   private:
    float m_aspectRatio;
    float m_zoom{1.0f};
    OrthographicCamera m_camera;

    bool m_rotationEnabled;

    glm::vec3 m_cameraPosition;
    float m_rotation;
    float m_translateSpeed = 5.0f, m_rotationSpeed = 180.0f;
};

}  // namespace Atlas