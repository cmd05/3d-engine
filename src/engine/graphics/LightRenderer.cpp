#include <engine/graphics/LightRenderer.hpp>
#include <engine/shaders/interface/ShaderDataTypes.hpp>

LightRenderer::LightRenderer() {
    m_light_cube_shader = std::make_unique<Shader>(std::string(FS_SHADERS_DIR) + "light_cube.vs", std::string(FS_SHADERS_DIR) + "light_cube.fs");
}

void LightRenderer::draw_light_cube(Components::Transform& transform, const CameraWrapper& camera_wrapper, glm::vec3 light_color) {
    m_light_cube_shader->activate();

    ShaderDataTypes::MeshMatrices mesh_matrices;
    mesh_matrices.model_matrix = glm::mat4(1.0f);
    mesh_matrices.mvp_matrix = GraphicsHelper::get_mvp_matrix(transform, camera_wrapper);
    mesh_matrices.normal_matrix = glm::mat4(1.0f);

    m_light_cube_shader->set_uniform<glm::mat4>("u_mesh_matrices.mvp_matrix", mesh_matrices.mvp_matrix);
    m_light_cube_shader->set_uniform<glm::vec3>("lightColor", light_color);

    glBindVertexArray(g_graphics_objects.cube.VAO);
    glDrawArrays(GL_TRIANGLES, 0, g_graphics_objects.cube.num_vertices);
}