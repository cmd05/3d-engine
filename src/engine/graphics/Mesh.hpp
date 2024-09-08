#pragma once

#include <engine/ecs/core/Types.hpp>
#include <engine/graphics/Shader.hpp>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <vector>
#include <memory>

struct MeshVertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 tex_coords;
    glm::vec3 tangent;
    glm::vec3 bitangent;
};

enum MeshTextureTypes {
    DIFFUSE,
    SPECULAR,
    NORMAL,
    AMBIENT
};

struct MeshTexture {
    unsigned int id;
    MeshTextureTypes type;
    std::string path;
};

class Mesh {
public:
    // mesh data
    std::vector<MeshVertex> m_vertices;
    std::vector<unsigned int> m_indices;
    std::vector<MeshTexture> m_textures;
    unsigned int VAO;

    // constructor
    Mesh(std::vector<MeshVertex> vertices, std::vector<unsigned int> indices, std::vector<MeshTexture> textures);

    // render the mesh
    void draw(std::unique_ptr<Shader>& shader);
private:
    unsigned int VBO, EBO;

    void setup_mesh();
};