#include "Border.h"

Border::Border(glm::vec3 start, glm::vec3 end) {
    m_start = start;
    m_end = end;

    m_center = (start + end) / 2.0f;
    m_length = glm::distance(start, end);
    m_radian = glm::orientedAngle(Vector3::right, glm::normalize(glm::vec3(end - start)), Vector3::up);
}

glm::vec3 Border::GetNearestPoint(glm::vec3 position) {
    glm::vec3 a = position - m_start;
    glm::vec3 b = m_end - m_start;
    float cos = glm::dot(a, b) / (glm::length(a) * glm::length(b));

    float length = glm::length(position - m_start) * cos; // Scaler projection

    if (length < 0)
        return m_start;
    else if (length > glm::length(m_end - m_start))
        return m_end;
    else
        return glm::normalize(m_end - m_start) * length + m_start;
}
