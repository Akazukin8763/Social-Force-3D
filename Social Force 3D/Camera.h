#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Utils.h"

class Camera {
private:
	glm::vec3 m_position;

	glm::vec3 m_up;
	glm::vec3 m_front;
	glm::vec3 m_right;
	glm::quat m_rotation;

	glm::mat4 m_projectionMatrix;
	glm::mat4 m_viewMatrix;

	float m_fov;
	float m_near;
	float m_far;

public:
	Camera() = default;
	Camera(glm::vec3 position);

	//void move(GLFWwindow* window);

	void UpdateViewMatrix();
	void UpdateProjectionMatrix(float aspect);

	const glm::mat4 GetProjectionMatrix() const { return m_projectionMatrix; }
	const glm::mat4 GetViewMatrix() const { return m_viewMatrix; }
	const glm::vec3 GetPosition() const { return m_position; }

	const float GetNearPlane() const { return m_near; }
	const float GetFarPlane() const { return m_far; }

	void HandleMovement(glm::vec3 movement);
	void HandleRotation(glm::quat rx, glm::quat ry);

};

#endif // !__CAMERA_H__