#include <memory>

#include <engine/graphics/MeshProcessor.hpp>

Mesh::Mesh(std::vector<MeshVertex> vertices, std::vector<unsigned int> indices, std::vector<MeshTexture> textures) {
    m_vertices = vertices;
    m_indices = indices;
    m_textures = textures;
}