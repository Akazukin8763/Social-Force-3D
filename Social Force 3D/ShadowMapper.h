#ifndef __SHADOW_MAPPER_H__
#define __SHADOW_MAPPER_H__

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"
#include "Model.h"
#include "Utils.h"

#include <vector>

class ShadowMapper {
private:
	int m_screenWidth, m_screenHeight;
	int m_shadowMapWidth, m_shadowMapHeight;

	float m_nearPlane = 0.1f, m_farPlane = 100.0f;

	Shader m_shadowShader;

	std::vector<GLuint> DirectionLightShadowMapFBO;
	std::vector<GLuint> DirectionLightShadowMap;
	std::vector<glm::mat4> DirectionLghtSpaceMatrix;

public:
	ShadowMapper() = default;
	ShadowMapper(const int screenWidth, const int screenHeight, 
				 const int shadowMapWidth, const int shadowMapHeight, 
				 const float nearPlane, const float farPlane);

	void Setup();

	void UpdateScreenSize(int width, int height);
	void UpdateNearFarPlane(float near, float far);

	void RenderDirectionLightShadowMap(glm::vec3 direction, std::vector<Model*> models);

	std::vector<glm::mat4> GetDirectionLightSpaceMatrix() { return DirectionLghtSpaceMatrix; }
	std::vector<GLuint> GetDirectionLightShadowMap() { return DirectionLightShadowMap; }

};

#endif