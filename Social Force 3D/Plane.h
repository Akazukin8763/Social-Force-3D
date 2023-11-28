#ifndef __PLANE_H__
#define __PLANE_H__

#include "Shader.h"

#include <vector>

class Plane {
private:
	unsigned int VAO, VBO;

	std::vector<glm::vec3> m_vertices;

	void Setup();

public:
	Plane() = default;
	Plane(int chunks);

	void Render(Shader &shader);

};

#endif // !__PLANE_H__
