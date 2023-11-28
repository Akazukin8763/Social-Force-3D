#ifndef __BORDER_H__
#define __BORDER_H__

#include <glm/glm.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"
#include "Model.h"
#include "Utils.h"

#include <vector>

class Border {
private:
    glm::vec3 m_start, m_end;

    glm::vec3 m_center;
    float m_length, m_radian;

public:
    Border(glm::vec3 start, glm::vec3 end);

    std::pair<glm::vec3, glm::vec3> GetPoints() { return std::make_pair(m_start, m_end); }
    glm::vec3 GetPosition() { return m_center; }
    float GetLength() { return m_length; }
    float GetRadian() { return m_radian; }

    glm::vec3 GetNearestPoint(glm::vec3 position);
};

#endif // !__BORDER_H__
