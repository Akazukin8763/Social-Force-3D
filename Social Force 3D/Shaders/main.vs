#version 460 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;

layout(location = 5) in ivec4 boneIds; 
layout(location = 6) in vec4 weights;

layout(location = 7) in mat4 InstanceModelMatrix;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
const int MAX_DIRICTION_LIGHT = 16;

out VertexData {
	vec3 position;
	vec2 texCoord;
	vec3 normal;

    vec4 positionDirectionLightSpace[MAX_DIRICTION_LIGHT];
} vs_out;

uniform mat4 Projection;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;

uniform bool useAnimation;
uniform mat4 FinalBonesMatrices[MAX_BONES];

uniform int DirectionLightNums;
uniform mat4 DirectionLightSpaceMatrices[MAX_DIRICTION_LIGHT];

void main() {
	// Animations
	vec4 totalPosition = vec4(0.0f);

	if (useAnimation) {
		for (int i = 0 ; i < MAX_BONE_INFLUENCE; i++) {
			if (boneIds[i] == -1) 
				continue;

			if (boneIds[i] >= MAX_BONES) {
				totalPosition = vec4(position,1.0f);
				break;
			}

			vec4 localPosition = FinalBonesMatrices[boneIds[i]] * vec4(position, 1.0);
			totalPosition += localPosition * weights[i];
			vec3 localNormal = mat3(FinalBonesMatrices[boneIds[i]]) * normal;
	   }
	}
	else {
		totalPosition = vec4(position, 1.0f);
	}

	// Instances
	mat4 FinalModelMatrix = InstanceModelMatrix * ModelMatrix;
	
	vs_out.position = vec3(FinalModelMatrix * totalPosition);
	vs_out.texCoord = texCoord;
	vs_out.normal = mat3(transpose(inverse(FinalModelMatrix))) * normal;

	for (int i = 0; i < DirectionLightNums; i++) {
		vs_out.positionDirectionLightSpace[i] = DirectionLightSpaceMatrices[i] * FinalModelMatrix * vec4(position, 1.0);
	}
	
	//
	gl_Position = Projection * ViewMatrix * FinalModelMatrix * totalPosition;
}
