#ifndef __MODEL_H__
#define __MODEL_H__

#include <glad/glad.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Shader.h"
#include "Vertex.h"
#include "Mesh.h"
#include "Texture.h"
#include "Animdata.h"
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

	// Model Loading
	void LoadModel(std::string const &path);
	
	void ProcessNode(aiNode *node, const aiScene *scene);
	Mesh ProcessMesh(aiMesh *mesh, const aiScene *scene);
	std::vector<Texture> LoadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName, const aiScene* scene);
	
	// Model Animation
	void SetVertexBoneDataToDefault(Vertex& vertex);
	void SetVertexBoneData(Vertex& vertex, int boneID, float weight);
	void ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene);

	//
	bool m_animationFlag = false;
	std::map<std::string, BoneInfo> m_boneInfoMap;
	int m_boneCounter = 0;

	glm::mat4 m_modelMatrix = Matrix4::identity;

	bool m_shadowFlag = false;

public:
	std::vector<Mesh>    m_meshes;
	std::vector<Texture> m_textures_loaded;

	bool m_gammaCorrection;

	// Constructor
	Model() = default;
	Model(std::string const &path, bool gamma = false);

	void SetAnimation(bool value) { m_animationFlag = value; }
	bool GetAnimation() { return m_animationFlag; }

	auto& GetBoneInfoMap() { return m_boneInfoMap; }
	int& GetBoneCount() { return m_boneCounter; }

	void SetModelMatrix(glm::mat4 modelMatrix) { m_modelMatrix = modelMatrix; }
	glm::mat4 GetModelMatrix() { return m_modelMatrix; }

	void SetShadow(bool value) { m_shadowFlag = value; }
	bool GetShadow() { return m_shadowFlag; }
    
	// Render the model
	void Render(Shader &shader) {
		// Setup Animations
		shader.SetBool("useAnimation", m_animationFlag);
		
		// Render the meshes
		for (unsigned int i = 0; i < m_meshes.size(); i++)
			m_meshes[i].Render(shader);
	}

	void UpdateInstanceTransforms(std::vector<glm::mat4> instances) {
		for (unsigned int i = 0; i < m_meshes.size(); i++)
			m_meshes[i].UpdateInstanceTransforms(instances);
	}

	void UpdateInstanceBonesMatrices(std::vector<std::vector<glm::mat4>> instances) {
		for (unsigned int i = 0; i < m_meshes.size(); i++)
			m_meshes[i].UpdateInstanceBonesMatrices(instances);
	}

	void UpdateShadowMaps(std::vector<GLuint> shadowMaps) {
		for (unsigned int i = 0; i < m_meshes.size(); i++)
			m_meshes[i].UpdateShadowMaps(shadowMaps);
	}

};

#endif