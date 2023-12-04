#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma) {
	std::string filename = std::string(path);
	filename = directory + '/' + filename;

	int width, height, nrComponents;
	unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	
	return GenerateTexture(data, width, height, nrComponents);
}

unsigned int TextureFrom_FBX_EmbeddedTexture(const aiTexture* tex, bool gamma) {
	int width, height, nrComponents;
	unsigned char *data = nullptr;

	if (tex->mHeight == 0) {
		data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(tex->pcData), tex->mWidth, &width, &height, &nrComponents, 0);
	}
	else {
		data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(tex->pcData), tex->mWidth * tex->mHeight, &width, &height, &nrComponents, 0);
	}

	return GenerateTexture(data, width, height, nrComponents);
}

unsigned int GenerateTexture(unsigned char* data, int width, int height, int nrComponents, bool gamma) {
	unsigned int textureID;
	glGenTextures(1, &textureID);

	if (data) {
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else {
		//std::cout << "Texture failed to load at path: " << path << "\n";
		std::cout << "Texture failed to load\n";
		stbi_image_free(data);
	}

	return textureID;
}