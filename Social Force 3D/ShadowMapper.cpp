#include "ShadowMapper.h"

ShadowMapper::ShadowMapper(const int screenWidth, const int screenHeight, 
						   const int shadowMapWidth, const int shadowMapHeight, 
						   const float nearPlane, const float farPlane) {
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;
	m_shadowMapWidth = shadowMapWidth;
	m_shadowMapHeight = shadowMapHeight;

	m_nearPlane = nearPlane;
	m_farPlane = farPlane;

	m_shadowShader = Shader();
}

void ShadowMapper::Setup() {
	m_shadowShader = Shader("./Shaders/directionLightShadowMap.vs", "./Shaders/directionLightShadowMap.fs");
	const float clampColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

	// Create Shadow Map
	DirectionLightShadowMapFBO = std::vector<GLuint>(CASCADES_NUMS);
	DirectionLightShadowMap = std::vector<GLuint>(CASCADES_NUMS);
	DirectionLghtSpaceMatrix = std::vector<glm::mat4>(CASCADES_NUMS);
	 
	for (int i = 0; i < CASCADES_NUMS; i++) {
		// Framebuffer for Shadow Map
		glGenFramebuffers(1, &DirectionLightShadowMapFBO[i]);

		// Texture for Shadow Map FBO
		glGenTextures(1, &DirectionLightShadowMap[i]);
		glBindTexture(GL_TEXTURE_2D, DirectionLightShadowMap[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 
					 m_shadowMapWidth, m_shadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, clampColor); // Prevents darkness outside the frustrum

		glBindFramebuffer(GL_FRAMEBUFFER, DirectionLightShadowMapFBO[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, DirectionLightShadowMap[i], 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

void ShadowMapper::UpdateScreenSize(int width, int height) {
	m_screenWidth = width;
	m_screenHeight = height;
}

void ShadowMapper::UpdateNearFarPlane(float near, float far) {
	m_nearPlane = near;
	m_farPlane = far;
}

void ShadowMapper::RenderDirectionLightShadowMap(glm::vec3 direction, std::vector<Model*> models) {
	direction = glm::normalize(direction);

	// Preparations for the Shadow Map
	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, m_shadowMapWidth, m_shadowMapHeight);

	for (int i = 0; i < CASCADES_NUMS; i++) {
		//float ratio = static_cast<float>(i + 1) / CASCADES_NUMS;
        //float near = ratio * (m_farPlane - m_nearPlane) + m_nearPlane;
        //float far = (ratio + 1.0f) * (m_farPlane - m_nearPlane) + m_nearPlane;
        float near = m_nearPlane;
        float far = m_farPlane;

		glm::vec3 lightPosition = -direction * 10.0f;
		glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -10.0f, 10.0f, near, far);
		glm::mat4 lightView = glm::lookAt(lightPosition, Vector3::zero, Vector3::up);
		DirectionLghtSpaceMatrix[i] = lightProjection * lightView;

		// Render Shadow Map
		m_shadowShader.Activate();

		glBindFramebuffer(GL_FRAMEBUFFER, DirectionLightShadowMapFBO[i]);
		glClear(GL_DEPTH_BUFFER_BIT);

		m_shadowShader.SetMat4("LightSpaceMatrix", DirectionLghtSpaceMatrix[i]);
		for (Model* model : models) {
			if (!model->GetShadow()) continue;

			m_shadowShader.SetMat4("ModelMatrix", model->GetModelMatrix()); // Local Coordinate
			model->Render(m_shadowShader); // Render
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	// Reset the viewport
	glViewport(0, 0, m_screenWidth, m_screenHeight);
}
