#version 460 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;

out VertexData {
	vec3 position;
} vs_out;

uniform mat4 Projection;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;

void main() {
	gl_Position = Projection * ViewMatrix * ModelMatrix * vec4(position, 1.0);

	vs_out.position = vec3(ModelMatrix * vec4(position, 1.0f));
}