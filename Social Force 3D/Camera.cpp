#include "Camera.h"

Camera::Camera(glm::vec3 position) {
    m_position = position;

    m_up = Vector3::up;
    m_front = Vector3::back; // -w direction
    m_right = Vector3::right;
    m_rotation = glm::identity<glm::quat>();

    m_projectionMatrix = Matrix4::identity;
    m_viewMatrix = Matrix4::identity;

    m_fov = 90.0f;
    m_near = 0.1f;
    m_far = 200.0f;
}

void Camera::UpdateViewMatrix() {
    m_front = m_rotation * Vector3::back;
    m_up = m_rotation * Vector3::up;
    m_right = glm::cross(m_front, m_up);

    m_viewMatrix = glm::lookAt(m_position, m_position + m_front, m_up);
}

void Camera::UpdateProjectionMatrix(float aspect) {
    m_projectionMatrix = glm::perspective(m_fov, aspect, m_near, m_far);
}

void Camera::HandleMovement(glm::vec3 movement) { 
    m_position += m_front * movement.z;
    m_position += Vector3::up * movement.y;
    m_position += m_right * movement.x;
    UpdateViewMatrix();
}

void Camera::HandleRotation(glm::quat rx, glm::quat ry) { 
    m_rotation = rx * m_rotation * ry;
    UpdateViewMatrix();
}
