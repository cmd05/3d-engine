#pragma once

#include <glm/glm.hpp>
#include <assimp/postprocess.h>
#include <engine/graphics/MeshProcessor.hpp>

struct GraphicsConfig {
    struct Camera { 
        // camera projection values
        static constexpr float DEFAULT_CAM_VFOV = glm::radians(45.0f);
        static constexpr float DEFAULT_CAM_NEAR_CLIP = 0.1;
        static constexpr float DEFAULT_CAM_FAR_CLIP = 1000;

        // camera direction vectors
        static constexpr glm::vec3 DEFAULT_CAM_WORLD_UP = glm::vec3(0.0f, 1.0f, 0.0f);
        static constexpr glm::vec3 DEFAULT_CAM_FRONT = glm::vec3(0.0f, 0.0f, -1.0f);
        static constexpr glm::vec3 DEFAULT_CAM_UP = DEFAULT_CAM_WORLD_UP;

        // camera movement
        static constexpr float CAMERA_PITCH_LIMIT = glm::radians(89.0f);
        static constexpr float CAMERA_VFOV_MIN = glm::radians(1.0f);
        static constexpr float CAMERA_VFOV_MAX = glm::radians(45.0f);

        // camera input
        static constexpr double CAMERA_MOUSE_SENSITIVITY = 0.05;
        static constexpr double CAMERA_SCROLL_SENSITIVITY = 1.0;
        static constexpr float CAMERA_SPEED = 90.0f;

        // camera position (stored in `Transform`)
        static constexpr glm::vec3 DEFAULT_CAM_POS = glm::vec3(90.0f, 50.0f, 10.0f); /* glm::vec3(0.0f) */
    };

    static constexpr const char* MODEL_BIN_PATH = FS_RESOURCES_DIR "models/bin";
    static constexpr const char* MODEL_BIN_MAPPER_PATH = FS_RESOURCES_DIR "models/bin/bin_mapper.map";
    static constexpr auto ASSIMP_READ_FLAGS = (aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_OptimizeMeshes);

    static constexpr MeshTextureType NO_GAMMA_CORRECT_MAPS = MeshTextureType::NORMAL; // ( | MeshTextureType::MAP | ... ) 

    static constexpr glm::vec4 GL_CLEAR_COLOR {0.0f, 0.0f, 0.0f, 1.0f};
};