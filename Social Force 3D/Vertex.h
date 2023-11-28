#ifndef __VERTEX_H__
#define __VERTEX_H__

struct InstanceData {
	glm::mat4 transform;
};

struct Vertex {
	glm::vec3 position;
	glm::vec2 texCoord;
	glm::vec3 normal;
};

#endif