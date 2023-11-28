#version 460 core

in VertexData {
	vec3 position;
} fs_in;

out vec4 FragColor;

uniform vec3 color;

void main() {
	vec4 renderColor = vec4(color, 1.0f);

	// Render Color
	FragColor = renderColor;
}