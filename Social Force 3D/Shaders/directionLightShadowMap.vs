#version 460

layout(location = 0) in vec3 position;
//layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;
//layout(location = 3) in vec3 tangent;
//layout(location = 4) in vec3 bitangent;

layout(location = 5) in ivec4 boneIds; 
layout(location = 6) in vec4 weights;

layout(location = 7) in mat4 InstanceModelMatrix;

const int MAX_INSTANCES = 40;
const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;

layout(std430, binding = 0) buffer InstanceBonesBuffer {
    mat4 FinalBonesMatrices[MAX_INSTANCES * MAX_BONES];
};

uniform mat4 ModelMatrix;
uniform mat4 LightSpaceMatrix;

uniform bool useAnimation;

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

			vec4 localPosition = FinalBonesMatrices[gl_InstanceID * MAX_BONES + boneIds[i]] * vec4(position, 1.0);
			totalPosition += localPosition * weights[i];
			vec3 localNormal = mat3(FinalBonesMatrices[gl_InstanceID * MAX_BONES + boneIds[i]]) * normal;
	   }
	}
	else {
		totalPosition = vec4(position, 1.0f);
	}

	// Instances
	mat4 FinalModelMatrix = InstanceModelMatrix * ModelMatrix;

	//
	gl_Position = LightSpaceMatrix * FinalModelMatrix * totalPosition;
}
