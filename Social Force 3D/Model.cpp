#include "Model.h"

Model::Model(std::string const &path, bool gamma)  {
	m_gammaCorrection = gamma;

	LoadModel(path);
}

/*
 *  Model Loading
 */
void Model::LoadModel(std::string const &path) {
	unsigned int assimp_read_flag = aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace;

	// read file via ASSIMP
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, assimp_read_flag);

	// Check for errors
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) { // if is Not Zero
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << "\n";
		return;
	}

	// Retrieve the directory path of the filepath
	m_directory = path.substr(0, path.find_last_of('/'));

	// Process ASSIMP's root node recursively
	ProcessNode(scene->mRootNode, scene);
}

void Model::ProcessNode(aiNode *node, const aiScene *scene) {
	// Process each mesh located at the current node
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		m_meshes.push_back(ProcessMesh(mesh, scene));
	}

	// After we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		ProcessNode(node->mChildren[i], scene);
	}
}

Mesh Model::ProcessMesh(aiMesh *mesh, const aiScene *scene) {
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	glm::vec3 tempVec3;
	glm::vec2 tempVec2;

	// Walk through the vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;
		SetVertexBoneDataToDefault(vertex);

		// Positions
		tempVec3.x = mesh->mVertices[i].x;
		tempVec3.y = mesh->mVertices[i].y;
		tempVec3.z = mesh->mVertices[i].z;
		vertex.Position = tempVec3;

		// Normals
		if (mesh->HasNormals()) {
			tempVec3.x = mesh->mNormals[i].x;
			tempVec3.y = mesh->mNormals[i].y;
			tempVec3.z = mesh->mNormals[i].z;
			vertex.Normal = tempVec3;
		}

		// Texture Coordinates
		if (mesh->mTextureCoords[0]) {
			// Texture Coordinate
			tempVec2.x = mesh->mTextureCoords[0][i].x; 
			tempVec2.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoord = tempVec2;

			// Tangent
			tempVec3.x = mesh->mTangents[i].x;
			tempVec3.y = mesh->mTangents[i].y;
			tempVec3.z = mesh->mTangents[i].z;
			vertex.Tangent = tempVec3;

			// Bitangent
			tempVec3.x = mesh->mBitangents[i].x;
			tempVec3.y = mesh->mBitangents[i].y;
			tempVec3.z = mesh->mBitangents[i].z;
			vertex.Bitangent = tempVec3;
		}
		else
			vertex.TexCoord = Vector2::zero;

		vertices.push_back(vertex);
	}

	// Walk through the mesh's face to get the vertex indicess
	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];

		for(unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);        
	}
	
	// Materials
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

	// 1. ambient maps
	std::vector<Texture> ambientMaps = LoadMaterialTextures(material, aiTextureType_AMBIENT, "textures.ambient", scene);
	textures.insert(textures.end(), ambientMaps.begin(), ambientMaps.end());
	// 2. diffuse maps
	std::vector<Texture> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "textures.diffuse", scene);
	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	// 3. specular maps
	std::vector<Texture> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "textures.specular", scene);
	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	// 4. normal maps
	std::vector<Texture> normalMaps = LoadMaterialTextures(material, aiTextureType_HEIGHT, "textures.normal", scene);
	textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	// 5. height maps
	std::vector<Texture> heightMaps = LoadMaterialTextures(material, aiTextureType_AMBIENT, "textures.height", scene);
	textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

	ExtractBoneWeightForVertices(vertices, mesh, scene);

	// Return a mesh object created from the extracted mesh data
	return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::LoadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName, const aiScene* scene) {
	std::vector<Texture> textures;

	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
		aiString str;
		mat->GetTexture(type, i, &str);

		// Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		bool skip = false;
		for (unsigned int j = 0; j < m_textures_loaded.size(); j++) {
			if (std::strcmp(m_textures_loaded[j].path.data(), str.C_Str()) == 0) {
				textures.push_back(m_textures_loaded[j]);
				skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
				break;
			}
		}

		if (!skip) { // If texture hasn't been loaded already, load it
			Texture texture;

			const aiTexture* tex = scene->GetEmbeddedTexture(str.C_Str());

			if (tex != nullptr)
				texture.id = TextureFrom_FBX_EmbeddedTexture(tex);
			else 
				texture.id = TextureFromFile(str.C_Str(), m_directory);

			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			m_textures_loaded.push_back(texture); // Store it as texture loaded for entire model, to ensure we won't unnecessary load duplicate textures.
		}
	}

	return textures;
}

/*
 *  Model Animation
 */
void Model::SetVertexBoneDataToDefault(Vertex& vertex) {
	for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
		vertex.m_BoneIDs[i] = -1;
		vertex.m_Weights[i] = 0.0f;
	}
}

void Model::SetVertexBoneData(Vertex& vertex, int boneID, float weight) {
	for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
		if (vertex.m_BoneIDs[i] < 0) {
			vertex.m_Weights[i] = weight;
			vertex.m_BoneIDs[i] = boneID;
			break;
		}
	}
}

void Model::ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene) {
	auto& boneInfoMap = m_boneInfoMap;
	int& boneCount = m_boneCounter;

	for (int boneIndex = 0; boneIndex < mesh->mNumBones; boneIndex++) {
		int boneID = -1;
		std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();

		if (boneInfoMap.find(boneName) == boneInfoMap.end()) {
			BoneInfo newBoneInfo;
			newBoneInfo.id = boneCount;
			newBoneInfo.offset = Utils::ConvertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix);
			boneInfoMap[boneName] = newBoneInfo;
			boneID = boneCount;
			boneCount++;
		}
		else {
			boneID = boneInfoMap[boneName].id;
		}

		assert(boneID != -1);
		auto weights = mesh->mBones[boneIndex]->mWeights;
		int numWeights = mesh->mBones[boneIndex]->mNumWeights;

		for (int weightIndex = 0; weightIndex < numWeights; weightIndex++) {
			int vertexId = weights[weightIndex].mVertexId;
			float weight = weights[weightIndex].mWeight;
			assert(vertexId <= vertices.size());
			SetVertexBoneData(vertices[vertexId], boneID, weight);
		}
	}
}