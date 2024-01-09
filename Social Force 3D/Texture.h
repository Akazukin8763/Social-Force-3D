#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include <glad/glad.h>

#include <assimp/scene.h>

#include <iostream>
#include <vector>

struct Texture {
	unsigned int id;
	std::string type;
	std::string path;
};

unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma = false);
unsigned int TextureFrom_FBX_EmbeddedTexture(const aiTexture* tex, bool gamma = false);

unsigned int GenerateTexture(unsigned char* data, int width, int height, int nrComponents, bool gamma = false);

unsigned int loadCubemap(std::vector<std::string> faces);
#endif