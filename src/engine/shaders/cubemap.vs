#version 460 core
layout (location = 0) in vec3 in_pos;

out vec3 TexCoords;

uniform mat4 u_VP_matrix;

void main() {
    TexCoords = in_pos;
    vec4 pos = u_VP_matrix * vec4(in_pos, 1.0);
    gl_Position = pos.xyww; // clip space coordinates
                            // since we have set zc = wc, clipping does not occur for z-value
}