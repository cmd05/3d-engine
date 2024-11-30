layout (std140, binding = 0) uniform ub_matrices {
    uniform mat4 u_view_matrix;
    uniform mat4 u_projection_matrix;
};