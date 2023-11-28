#ifndef __MODEL_H__
#define __MODEL_H__

#include <glad/glad.h>

#include <stb_image.h>

#include "Shader.h"
#include "Vertex.h"
#include "Mesh.h"
#include "Texture.h"
#include "Utils.h"

#include <regex>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>

class Model {
private:
	std::string m_directory;

	void ReadMtlFile(std::string filepath);
	void ReadObjFile(std::string filepath);
	unsigned int LoadTextureFile(std::string filename);

	//
	glm::mat4 m_modelMatrix = Matrix4::identity;

	bool m_shadowFlag = false;

public:
	std::vector<Mesh> m_meshes;
	std::map<std::string, Texture> m_textures;

	Model() = default;
	Model(std::string filepath);
	
	void SetModelMatrix(glm::mat4 modelMatrix) { m_modelMatrix = modelMatrix; }
	glm::mat4 GetModelMatrix() { return m_modelMatrix; }

	void SetShadow(bool value) { m_shadowFlag = value; }
	bool GetShadow() { return m_shadowFlag; }

	void UpdateInstanceDatas(std::vector<InstanceData> instances);
	void UpdateShadowMaps(std::vector<GLuint> shadowMaps);

	void Render(Shader &shader);
};

#endif