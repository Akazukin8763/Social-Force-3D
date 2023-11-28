#version 460 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;

layout(location = 3) in mat4 transform;

out VertexData {
	vec3 position;
	vec2 texCoord;
	vec3 normal;

    vec4 positionDirectionLightSpace[16];
} vs_out;

uniform mat4 Projection;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;

uniform mat4 DirectionLightSpaceMatrix[16];
uniform int DirectionLightSpaceMatrixNums;

void main() {
	mat4 newModelMatrix = ModelMatrix * transform;

	//
	gl_Position = Projection * ViewMatrix * newModelMatrix * vec4(position, 1.0);
	
	vs_out.position = vec3(newModelMatrix * vec4(position, 1.0));
	vs_out.texCoord = texCoord;
	vs_out.normal = mat3(transpose(inverse(newModelMatrix))) * normal;

	for (int i = 0; i < DirectionLightSpaceMatrixNums; i++) {
		vs_out.positionDirectionLightSpace[i] = DirectionLightSpaceMatrix[i] * newModelMatrix * vec4(position, 1.0);
	}
}