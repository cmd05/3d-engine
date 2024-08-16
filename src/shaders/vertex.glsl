#version 330 core

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

out vec3 normal;
out vec3 fragPosWorld;

void main()
{
	fragPosWorld = vec3(uModel * vec4(inPos, 1.0));
	normal = vec3(uModel * vec4(inNormal, 0.0));

	gl_Position = uProjection * uView * uModel * vec4(inPos, 1.0f);
}
