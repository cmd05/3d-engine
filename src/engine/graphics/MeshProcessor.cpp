#include <memory>

#include <engine/graphics/MeshProcessor.hpp>

Mesh::Mesh(std::vector<MeshVertex> vertices, std::vector<unsigned int> indices, std::vector<MeshTexture> textures,
    MeshTexturesAvailable mesh_textures_available) {
    m_vertices = vertices;
    m_indices = indices;
    m_textures = textures;
    m_textures_available = mesh_textures_available;
}