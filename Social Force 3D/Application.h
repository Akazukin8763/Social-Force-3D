#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <bits/stdc++.h>

#include "Shader.h"
#include "ShadowMapper.h"
#include "Camera.h"
#include "SocialForce.h"
#include "Model.h"
#include "PedestrianAnimator.h"
#include "Plane.h"
#include "Utils.h"

class Application {
private:
	GLFWwindow* m_window;

	Shader m_mainShader, m_planeShader, m_skyboxShader;
	ShadowMapper m_shadowMapper;

	int m_screenWidth, m_screenHeight;
	const unsigned int m_shadowMapWidth = 1024, m_shadowMapHeight = 1024;

	float m_currentTime, m_lastTime, m_deltaTime;
	float m_smoothedFPS = 0.0f, m_smoothingFactor = 0.005f;

	Camera m_camera;

	SocialForce m_socialForce;
	Model m_pedestrianModel, m_borderModel;
	Plane m_plane;

	glm::vec3 m_cameraTranslate = Vector3::zero;

	// GUI 
	bool m_toggleGUI = false;

	bool m_simulateFlag = false;

	bool m_weightFlag = false;
	bool m_collisionFlag = false;

	glm::vec3 m_directionLightDirection = glm::vec3(0.0, -1.0, -1.0);
	bool m_directionLightShadowFlag = false;

	int m_pedestrianNums = MAX_INSTANCES;

	float skyboxVertices[108] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	// Setup
	bool Setup();
	void Rendering();
	void ClearResource();

	// Callback Function
	void OnReshape(GLFWwindow* window, int width, int height);
	void OnMouse(GLFWwindow* window, int button, int action, int mods);
	void OnMouseMovement(GLFWwindow* window, double xpos, double ypos);
	void OnMouseScroll(GLFWwindow* window, double xoffset, double yoffset);
	void OnKeyBoard(GLFWwindow* window, int key, int scancode, int action, int mods);

	class GLFWCallbackWrapper {
	private:
		static Application* s_application;

	public:
		GLFWCallbackWrapper() = delete;
		GLFWCallbackWrapper(const GLFWCallbackWrapper&) = delete;
		GLFWCallbackWrapper(GLFWCallbackWrapper&&) = delete;
		~GLFWCallbackWrapper() = delete;

		static void OnReshape(GLFWwindow* window, int width, int height);
		static void OnMouse(GLFWwindow* window, int button, int action, int mods);
		static void OnMouseMovement(GLFWwindow* window, double xpos, double ypos);
		static void OnMouseScroll(GLFWwindow* window, double xoffset, double yoffset);
		static void OnKeyBoard(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void SetApplication(Application *application);
	};

public:
	Application(const int width, const int height);

	void Execute();

};

#endif // !__APPLICATION_H__