#ifndef __TEXTURE_H__
#define __TEXTURE_H__

struct Texture {
	unsigned int id;

	glm::vec3 Ka; // ambient
	glm::vec3 Kd; // diffuse
	glm::vec3 Ks; // specular
	float shininess;
};

#endif