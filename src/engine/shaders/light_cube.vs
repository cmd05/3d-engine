#version 460 core
layout (location = 0) in vec3 in_pos;

#include "include/data_types.glsl"

uniform MeshMatrices u_mesh_matrices;

void main() {
    gl_Position = u_mesh_matrices.mvp_matrix * vec4(in_pos, 1.0);
}