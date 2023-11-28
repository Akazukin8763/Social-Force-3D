#include "Model.h"

Model::Model(std::string filepath) {
	m_directory = filepath.substr(0, filepath.find_last_of('/') + 1);

	ReadObjFile(filepath);
}

void Model::ReadObjFile(std::string filepath) {
	std::stringstream ss;
	std::ifstream in_file(filepath);
	std::string line = "";
	std::string prefix = "";

	if (!in_file.is_open()) {
		throw "Could not open the file.";
	}

	std::string mtlFileName;
	std::string currentMtlName;

	std::vector<glm::vec3> positions;
	std::vector<glm::vec2> texCoords;
	std::vector<glm::vec3> normals;
	
	bool firstload = true;

	unsigned int tempUint;
	Vertex tempVertex;

	std::vector<Vertex> vertices;
	
	while (std::getline(in_file, line)) {
		auto iter =  std::find_if(line.rbegin() , line.rend() , [](char ch){ return !std::isspace<char>(ch , std::locale::classic()); });
		line.erase(iter.base(), line.end()); // Remove only trailing spaces

		ss.clear();
		ss.str(line);
		ss >> prefix;

		if (prefix == "mtllib") {
			ss.ignore(1); // skip space
			std::getline(ss, mtlFileName);

			// Load material
			ReadMtlFile(m_directory + mtlFileName);
		}
		else if (prefix == "usemtl") {
			if (!firstload) {
				m_meshes.push_back(Mesh(vertices, m_textures[currentMtlName]));
				vertices.clear(); // Clear vertices for next mesh
			}
			firstload = false;

			ss >> currentMtlName;
		}
		else if (prefix == "v") { // Vertex Position
			ss >> tempVertex.position.x >> tempVertex.position.y >> tempVertex.position.z;
			positions.push_back(tempVertex.position);
		}
		else if (prefix == "vt") { // Vertex Texture Coordinate
			ss >> tempVertex.texCoord.x >> tempVertex.texCoord.y;
			texCoords.push_back(tempVertex.texCoord);
		}
		else if (prefix == "vn") { // Vertex Normal
			ss >> tempVertex.normal.x >> tempVertex.normal.y >> tempVertex.normal.z;
			normals.push_back(tempVertex.normal);
		}
		else if (prefix == "f") { // Face
			std::vector<Vertex> face;

			while (!ss.eof()) {
				ss >> tempUint;
				ss.ignore(1); // skip slash
				tempVertex.position = positions[tempUint - 1];

				ss >> tempUint;
				ss.ignore(1); // skip slash
				tempVertex.texCoord = texCoords[tempUint - 1];

				ss >> tempUint;
				ss.ignore(1); // skip space
				tempVertex.normal = normals[tempUint - 1];

				face.push_back(tempVertex);
			}

			if (face.size() == 3) {
				vertices.push_back(face[0]);
				vertices.push_back(face[1]);
				vertices.push_back(face[2]);
			}
			else if (face.size() == 4) {
				vertices.push_back(face[0]);
				vertices.push_back(face[1]);
				vertices.push_back(face[2]);
				vertices.push_back(face[2]);
				vertices.push_back(face[3]);
				vertices.push_back(face[0]);
			}
		}
	}

	m_meshes.push_back(Mesh(vertices, m_textures[currentMtlName]));
}

void Model::ReadMtlFile(std::string filepath) {
	std::stringstream ss;
	std::ifstream in_file(filepath);
	std::string line = "";
	std::string prefix = "";

	if (!in_file.is_open()) {
		throw "Could not open the file.";
	}

	std::string currentMtlName;

	glm::vec3 tempVec3;
	std::string tempString;
	float tempFloat;

	while (std::getline(in_file, line)) {
		ss.clear();
		ss.str(line);
		ss >> prefix;

		if (prefix == "newmtl") {
			ss >> currentMtlName;
			m_textures[currentMtlName] = Texture();
		}
		else if (prefix == "Ka") { // Ambient
			ss >> tempVec3.x >> tempVec3.y >> tempVec3.z;
			m_textures[currentMtlName].Ka = tempVec3;
		}
		else if (prefix == "Kd") { // Diffuse
			ss >> tempVec3.x >> tempVec3.y >> tempVec3.z;
			m_textures[currentMtlName].Kd = tempVec3;
		}
		else if (prefix == "Ks") { // Specular
			ss >> tempVec3.x >> tempVec3.y >> tempVec3.z;
			m_textures[currentMtlName].Ks = tempVec3;
		}
		else if (prefix == "Ns") { // Specular shininess
			ss >> tempFloat;
			m_textures[currentMtlName].shininess = tempFloat;
		}
		else if (prefix == "map_Ka") {} // Ambient texture map
		else if (prefix == "map_Kd") { // Diffuse texture map
			ss.ignore(1); // skip space
			std::getline(ss, tempString);
			m_textures[currentMtlName].id = LoadTextureFile(tempString);
		}
		else if (prefix == "map_Ks") {} // Specular texture map
		else if (prefix == "map_d") {} // Alpha texture map
	}
}

unsigned int Model::LoadTextureFile(std::string filename) {
	unsigned int id;
	glGenTextures(1, &id);

	int width, height, channels;
	stbi_set_flip_vertically_on_load(true); // Flip y-axis when loading
	unsigned char *texture = stbi_load((m_directory + '/' + filename).c_str(), &width, &height, &channels, 0);

	if (texture) {
		GLenum format;
		if (channels == 1) format = GL_RED;
		else if (channels == 3) format = GL_RGB;
		else if (channels == 4) format = GL_RGBA;

		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		glBindTexture(GL_TEXTURE_2D, id);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, texture);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(texture);

	return id;
}

void Model::UpdateInstanceDatas(std::vector<InstanceData> instances) {
	for (unsigned int i = 0; i < m_meshes.size(); i++)
		m_meshes[i].UpdateInstanceDatas(instances);
}

void Model::UpdateShadowMaps(std::vector<GLuint> shadowMaps) {
	for (unsigned int i = 0; i < m_meshes.size(); i++)
		m_meshes[i].UpdateShadowMaps(shadowMaps);
}

void Model::Render(Shader &shader) {
	for (unsigned int i = 0; i < m_meshes.size(); i++)
		m_meshes[i].Render(shader);
}