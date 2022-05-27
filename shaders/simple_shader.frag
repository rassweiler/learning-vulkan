#version 460

layout (location = 0) out vec4 outputColor;

layout(push_constant) uniform Push {
	mat2 transform;
	vec2 offset;
	vec3 color;
} push;

void main() {
	outputColor = vec4(push.color, 1.0);
}
