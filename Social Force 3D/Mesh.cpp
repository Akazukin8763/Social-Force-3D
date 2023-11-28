#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex> vertices, Texture texture) {
    m_vertices = vertices;
    m_texture = texture;

    Setup();
}

void Mesh::Setup() {
    // Create Vertex Array Object, Vertex Buffer Object, Instance VBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &InstanceVBO);

    // Bind VAO, VBO, InstanceVBO and set attributes
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), &m_vertices[0], GL_STATIC_DRAW);

    // Vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // Vertex Texture Coords
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
    // Vertex Normals
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    glBindBuffer(GL_ARRAY_BUFFER, InstanceVBO);

    // Instance Transform Matrix
    for (int i = 0; i < 4; ++i) {
        glEnableVertexAttribArray(3 + i);
        //glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)(offsetof(InstanceData, transform) + sizeof(glm::vec4) * i));
        glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)(sizeof(glm::vec4) * i));
        glVertexAttribDivisor(3 + i, 1); // Update each instances
    }

    // Unbind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mesh::Render(Shader &shader) {
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, InstanceVBO);
    glBufferData(GL_ARRAY_BUFFER, m_instances.size() * sizeof(InstanceData), &m_instances[0], GL_STATIC_DRAW);

    // Activate material and texture
    shader.SetInt("material.ambient", m_texture.id);
    shader.SetInt("material.diffuse", m_texture.id);
    shader.SetInt("material.specular", m_texture.id);
    shader.SetVec3("material.Ka", m_texture.Ka);
    shader.SetVec3("material.Kd", Vector3::ones); // texture.Kd
    shader.SetVec3("material.Ks", m_texture.Ks);
    shader.SetFloat("material.shininess", m_texture.shininess);

    glActiveTexture(GL_TEXTURE0 + m_texture.id);
    glBindTexture(GL_TEXTURE_2D, m_texture.id);

    // Bind Shadow Map
    for (GLuint shadowMap : m_shadowMaps) {
        glActiveTexture(GL_TEXTURE0 + shadowMap);
        glBindTexture(GL_TEXTURE_2D, shadowMap);
    }

    // Render
    glDrawArraysInstanced(GL_TRIANGLES, 0, m_vertices.size(), m_instances.size());

    // Unbind texture
    glActiveTexture(GL_TEXTURE0);

    glBindVertexArray(0);
}