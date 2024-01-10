#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures, Material material) {
    m_vertices = vertices;
    m_indices = indices;
    m_textures = textures;
    m_material = material;

    Setup();
}

void Mesh::Setup() {
    // Create buffers/arrays
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &SSBO);
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

    // Instance Transform Matrix
    glBindBuffer(GL_ARRAY_BUFFER, InstanceVBO);
    for (int i = 0; i < 4; i++) {
        glEnableVertexAttribArray(7 + i);
        glVertexAttribPointer(7 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4) * i));
        glVertexAttribDivisor(7 + i, 1); // Update each instances
    }

    // Instance Bone Matrices
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, SSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::mat4) * MAX_INSTANCES * MAX_BONES, NULL, GL_DYNAMIC_DRAW);

    // Unbind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Mesh::Render(Shader &shader) {
    // Activate material and texture
    for (unsigned int i = 0; i < m_textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + m_textures[i].id); // active proper texture unit before binding

        std::string type = m_textures[i].type;
        shader.SetInt(type, m_textures[i].id);

        glBindTexture(GL_TEXTURE_2D, m_textures[i].id); // and finally bind the texture
    }

    shader.SetVec3("material.ambient", m_material.Ka);
    shader.SetVec3("material.diffuse", m_material.Kd);
    shader.SetVec3("material.specular", m_material.Ks);
    shader.SetFloat("material.shininess", 1);

    // Activate shadow map
    for (GLuint shadowMap : m_shadowMaps) {
        glActiveTexture(GL_TEXTURE0 + shadowMap);
        glBindTexture(GL_TEXTURE_2D, shadowMap);
    }

    // Render
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, InstanceVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * m_instanceTransforms.size(), m_instanceTransforms.data(), GL_STATIC_DRAW);

    std::vector<glm::mat4> flattenedData;
    for (const auto& bonesMatrices : m_instanceBonesMatrices) {
        flattenedData.insert(flattenedData.end(), bonesMatrices.begin(), bonesMatrices.end());
    }

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, SSBO);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(glm::mat4) * flattenedData.size(), flattenedData.data());

    glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(m_indices.size()), GL_UNSIGNED_INT, 0, m_instanceTransforms.size());

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    // Unbind texture
    glActiveTexture(GL_TEXTURE0);
}