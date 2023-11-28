#ifndef __MESH_H__
#define __MESH_H__

#include <glad/glad.h>

#include "Shader.h"
#include "Vertex.h"
#include "Texture.h"
#include "Utils.h"

#include <vector>

class Mesh {
private:
	unsigned int VAO, VBO, InstanceVBO;
	std::vector<InstanceData> m_instances;
	std::vector<GLuint> m_shadowMaps;

	void Setup();

public:
	std::vector<Vertex> m_vertices;
	Texture m_texture;

	Mesh(std::vector<Vertex> vertices, Texture texture);

	void UpdateInstanceDatas(std::vector<InstanceData> instances) { m_instances = instances; }
	void UpdateShadowMaps(std::vector<GLuint> shadowMaps) { m_shadowMaps = shadowMaps; }

	void Render(Shader &shader);
};

#endif