#include "Plane.h"

Plane::Plane(int chunks) {
    // Create Mesh Vertices
    std::vector<glm::vec3> vertices;
    float offset = 0.01f;

    for (int i = 0; i < chunks; i++) {
        vertices.push_back(glm::vec3(-chunks, offset, -chunks + i));
        vertices.push_back(glm::vec3(chunks, offset, -chunks + i));
        vertices.push_back(glm::vec3(-chunks, offset, chunks - i));
        vertices.push_back(glm::vec3(chunks, offset, chunks - i));
    }

    for (int i = 0; i < chunks; i++) {
        vertices.push_back(glm::vec3(-chunks + i, offset, chunks));
        vertices.push_back(glm::vec3(-chunks + i, offset, -chunks));
        vertices.push_back(glm::vec3(chunks - i, offset, chunks));
        vertices.push_back(glm::vec3(chunks - i, offset, -chunks));
    }

    vertices.push_back(glm::vec3(-chunks, offset, 0.0f));
    vertices.push_back(glm::vec3(chunks, offset, 0.0f));
    vertices.push_back(glm::vec3(0.0f, offset, -chunks));
    vertices.push_back(glm::vec3(0.0f, offset, chunks));

    vertices.push_back(glm::vec3(-chunks, 0.0f, -chunks));
    vertices.push_back(glm::vec3(-chunks, 0.0f, chunks));
    vertices.push_back(glm::vec3(chunks, 0.0f, -chunks));
    vertices.push_back(glm::vec3(chunks, 0.0f, chunks));

    m_vertices = vertices;

    Setup();
}

void Plane::Setup() {
    // Create Vertex Array Object, Vertex Buffer Object
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // Bind VAO, VBO and set attributes
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(glm::vec3), &m_vertices[0], GL_STATIC_DRAW);

    // Vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
}

void Plane::Render(Shader &shader) {
    glBindVertexArray(VAO);

    shader.SetVec3("color", glm::vec3(0.5f, 0.5f, 0.5f)); // White Mesh
    glDrawArrays(GL_LINES, 0, m_vertices.size() - 8);

    shader.SetVec3("color", glm::vec3(1.0f, 0.0f, 0.0f)); // Red X-axis
    glDrawArrays(GL_LINES, m_vertices.size() - 8, 2);

    shader.SetVec3("color", glm::vec3(0.0f, 0.0f, 1.0f)); // Blue Z-axis
    glDrawArrays(GL_LINES, m_vertices.size() - 6, 2);

    shader.SetVec3("color", glm::vec3(0.3f, 0.3f, 0.3f)); // Plane
    glDrawArrays(GL_TRIANGLE_STRIP, m_vertices.size() - 4, 4);

    glBindVertexArray(0);
}