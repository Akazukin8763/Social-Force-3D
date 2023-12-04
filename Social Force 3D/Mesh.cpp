#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures) {
    m_vertices = vertices;
    m_indices = indices;
    m_textures = textures;

    Setup();
}

void Mesh::Setup() {
    // Create buffers/arrays
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &InstanceVBO);

    // Binding
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), &m_vertices[0], GL_STATIC_DRAW);  

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), &m_indices[0], GL_STATIC_DRAW);

    // Vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // Vertex texture coords
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoord));
    // Vertex normals
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // Vertex tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
    // Vertex bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
    // Ids
    glEnableVertexAttribArray(5);
    glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));
    // Weights
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));

    glBindBuffer(GL_ARRAY_BUFFER, InstanceVBO);

    // Instance Transform Matrix
    for (int i = 0; i < 4; ++i) {
        glEnableVertexAttribArray(7 + i);
        //glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)(offsetof(InstanceData, transform) + sizeof(glm::vec4) * i));
        glVertexAttribPointer(7 + i, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)(sizeof(glm::vec4) * i));
        glVertexAttribDivisor(7 + i, 1); // Update each instances
    }

    // Unbind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mesh::Render(Shader &shader) {
    // Activate material and texture
    for (unsigned int i = 0; i < m_textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + m_textures[i].id); // active proper texture unit before binding

        std::string type = m_textures[i].type;
        shader.SetInt(type, m_textures[i].id);

        glBindTexture(GL_TEXTURE_2D, m_textures[i].id); // and finally bind the texture
    }

    shader.SetVec3("material.ambient", Vector3::ones);
    shader.SetVec3("material.diffuse", Vector3::ones);
    shader.SetVec3("material.specular", Vector3::ones);
    shader.SetFloat("material.shininess", 1);

    // Activate shadow map
    for (GLuint shadowMap : m_shadowMaps) {
        glActiveTexture(GL_TEXTURE0 + shadowMap);
        glBindTexture(GL_TEXTURE_2D, shadowMap);
    }

    // Render
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, InstanceVBO);
    glBufferData(GL_ARRAY_BUFFER, m_instances.size() * sizeof(InstanceData), &m_instances[0], GL_STATIC_DRAW);

    glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(m_indices.size()), GL_UNSIGNED_INT, 0, m_instances.size());

    glBindVertexArray(0);

    // Unbind texture
    glActiveTexture(GL_TEXTURE0);
}