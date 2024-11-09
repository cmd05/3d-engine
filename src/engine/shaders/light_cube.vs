#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 u_MVP;

void main() {
    gl_Position = u_MVP * vec4(aPos, 1.0);
}