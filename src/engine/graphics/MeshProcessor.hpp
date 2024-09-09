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

enum MeshTextureType {
    DIFFUSE,
    SPECULAR,
    NORMAL,
    AMBIENT
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
    std::vector<MeshTexture> m_textures;
    
    Mesh() {}
    Mesh(std::vector<MeshVertex> vertices, std::vector<unsigned int> indices, std::vector<MeshTexture> textures);
};