#pragma once

#include <engine/ecs/core/Types.hpp>

#include <glad/glad.h>

#include <glm/glm.hpp>

#include <string>
#include <vector>

struct MeshVertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 tex_coords;
    glm::vec3 tangent;
    glm::vec3 bitangent;
};

// use numbering as flags
enum MeshTextureType {
    DIFFUSE    = 1U << 0,
    SPECULAR   = 1U << 1,
    NORMAL     = 1U << 2,
    AMBIENT    = 1U << 3,
    METALLICROUGHNESS = 1U << 4,
    AMBIENT_OCCLUSION = 1U << 5,
    // METALLIC,
    // ROUGHNESS,
    // UNKNOWN // does not match any of the above types only then
};

struct MeshTexturesAvailable {
    unsigned int diffuse            = 0;
    unsigned int specular           = 0;
    unsigned int normal             = 0;
    unsigned int ambient            = 0;
    unsigned int metallic_roughness = 0;
    // unsigned int metallic           = 0;
    // unsigned int roughness          = 0;
    // unsigned int unknown = 0;
};

struct MeshTexture {
    MeshTextureType type;
    std::string path;
};

class Mesh {
public:
    // mesh data
    std::vector<MeshVertex> m_vertices;
    std::vector<unsigned int> m_indices;
    MeshTexturesAvailable m_textures_available;
    std::vector<MeshTexture> m_textures;

    Mesh() {}
    Mesh(std::vector<MeshVertex> vertices, std::vector<unsigned int> indices, 
        std::vector<MeshTexture> textures, MeshTexturesAvailable mesh_textures_available);
};