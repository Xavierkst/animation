#version 330 core

out vec4 fragColor;

uniform vec3 lightColor;
in vec3 fragPos;
in vec3 fragNormal;

void main() {

	fragColor = vec4(lightColor, 1.0f);	
}