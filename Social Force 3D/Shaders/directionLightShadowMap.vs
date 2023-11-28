#version 460

layout(location = 0) in vec3 position;
//layout(location = 1) in vec2 texCoord;
//layout(location = 2) in vec3 normal;

layout(location = 3) in mat4 transform;

uniform mat4 ModelMatrix;

uniform mat4 LightSpaceMatrix;

void main() {
	mat4 newModelMatrix = ModelMatrix * transform;

	//
	gl_Position = LightSpaceMatrix * newModelMatrix * vec4(position, 1.0);
}
