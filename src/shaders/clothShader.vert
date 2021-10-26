#version 330 core
// NOTE: Do NOT use any version older than 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

uniform mat4 viewProj;
uniform mat4 model;

out vec3 fragNormal;
out vec3 fragPos;
out vec2 tex_coord;

void main()
{
    gl_Position = viewProj * model * vec4(position, 1.0);
    fragPos = position; 
	fragNormal = vec3(inverse(transpose(model)) * vec4(normal, 0));
    tex_coord = texCoord;
}