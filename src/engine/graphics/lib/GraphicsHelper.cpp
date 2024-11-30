#include <engine/graphics/lib/GraphicsHelper.hpp>

glm::mat4 GraphicsHelper::create_model_matrix(const Components::Transform& transform) {
    glm::mat4 translation_mat = glm::translate(glm::mat4(1.0f), transform.position);
    glm::mat4 rot_mat = glm::mat4_cast(transform.rotation);
    glm::mat4 scale_mat = glm::scale(glm::mat4(1.0f), transform.scale);

    glm::mat4 model = translation_mat * rot_mat * scale_mat;

    return model;
}

GraphicsHelper::MVP GraphicsHelper::get_mvp(const Components::Transform& transform, const CameraWrapper& camera_wrapper) {
    MVP mvp;

    mvp.model = create_model_matrix(transform);
    mvp.view = camera_wrapper.get_view_matrix();
    mvp.projection = camera_wrapper.get_projection_matrix();

    return mvp;
}

glm::mat4 GraphicsHelper::get_mvp_matrix(const Components::Transform& transform, const CameraWrapper& camera_wrapper) {
    MVP mvp = get_mvp(transform, camera_wrapper);
    glm::mat4 mvp_matrix = mvp.projection * mvp.view * mvp.model;

    return mvp_matrix;
}