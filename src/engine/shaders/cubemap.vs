#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 u_VP_matrix;

void main() {
    TexCoords = aPos;
    vec4 pos = u_VP_matrix * vec4(aPos, 1.0);
    gl_Position = pos.xyww; // clip space coordinates
                            // since we have set zc = wc, clipping does not occur for z-value
}