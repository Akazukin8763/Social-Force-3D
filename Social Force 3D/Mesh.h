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
    unsigned int VAO, VBO, EBO, SSBO;

    unsigned int InstanceVBO;
    std::vector<glm::mat4> m_instanceTransforms;
    std::vector<std::vector<glm::mat4>> m_instanceBonesMatrices;
    std::vector<GLuint> m_shadowMaps;

    void Setup();

public:
    std::vector<Vertex>       m_vertices;
    std::vector<unsigned int> m_indices;
    std::vector<Texture>      m_textures;

    // Constructor
    Mesh() = default;
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);

    // Render the mesh
    void Render(Shader &shader);

    void UpdateInstanceTransforms(std::vector<glm::mat4> instances) { m_instanceTransforms = instances; }
    void UpdateInstanceBonesMatrices(std::vector<std::vector<glm::mat4>> instances) { m_instanceBonesMatrices = instances; }
    void UpdateShadowMaps(std::vector<GLuint> shadowMaps) { m_shadowMaps = shadowMaps; }

};

#endif