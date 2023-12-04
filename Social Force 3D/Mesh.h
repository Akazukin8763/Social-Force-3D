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
    unsigned int VAO, VBO, EBO;

    unsigned int InstanceVBO;
    std::vector<InstanceData> m_instances;
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

    void UpdateInstanceDatas(std::vector<InstanceData> instances) { m_instances = instances; }
    void UpdateShadowMaps(std::vector<GLuint> shadowMaps) { m_shadowMaps = shadowMaps; }

};

#endif