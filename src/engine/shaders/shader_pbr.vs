#version 460 core
layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_tex_coords;
layout (location = 3) in vec3 in_tangent;
layout (location = 4) in vec3 in_bitangent;

#include "include/data_types.glsl"
#include "include/ub_camera.glsl"

uniform MeshMatrices u_mesh_matrices;
uniform Light u_point_lights[NR_POINT_LIGHTS];

out VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 world_normal;
    vec3 view_pos;
    vec3 world_pos;

    mat3 TBN_matrix;
} vs_out;

void main() {
    // convert quantities for uniform blocks
    // ---------------------------------------------------
    vec3 view_pos = vec3(u_view_pos);

    vs_out.FragPos = vec3(u_mesh_matrices.model_matrix * vec4(in_pos, 1.0));
    vs_out.TexCoords = in_tex_coords;

    // calculate TBN matrix: tangent space to world space
    vec3 T = normalize(u_mesh_matrices.normal_matrix * in_tangent);
    vec3 N = normalize(u_mesh_matrices.normal_matrix * in_normal);

    // re-orthogonalize T with respect to N (gram-schmidt) 
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    mat3 TBN_matrix = mat3(T, B, N);
    vs_out.TBN_matrix = TBN_matrix;

    vs_out.world_normal = normalize(u_mesh_matrices.normal_matrix * in_normal);
    vs_out.view_pos = view_pos;
    vs_out.world_pos = vec3(u_mesh_matrices.model_matrix * vec4(in_pos, 1.0));

    gl_Position = u_mesh_matrices.mvp_matrix * vec4(in_pos, 1.0);
}