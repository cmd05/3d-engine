#include <glad/glad.h> 

#include <glm/glm.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <engine/graphics/ModelProcessor.hpp>

#include <engine/graphics/MeshProcessor.hpp>
#include <engine/graphics/Shader.hpp>

#include <lib/utilities/DebugAssert.hpp>

#include <string>
#include <memory>
#include <vector>

#include <algorithm>
#include <iterator>

void copy_assimp_vec3(glm::vec3& to, aiVector3D& from) {
    to.x = from.x;
    to.y = from.y;
    to.z = from.z;
}

ModelProcessor::ModelProcessor(const std::string& path, bool gamma): m_gamma_correction{gamma} {
    load_model(path);
}

void ModelProcessor::load_model(const std::string& path) {
    m_model_path = path;

    // read file via assimp
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, ASSIMP_READ_FLAGS);

    // check for errors
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) { // values should not be zero
        ASSERT(true, "ERROR::ASSIMP::" << importer.GetErrorString());
        return;
    }

    // process ASSIMP's root node recursively
    process_node(scene->mRootNode, scene);
}

// processes a node in a recursive fashion.
// Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
void ModelProcessor::process_node(aiNode* node, const aiScene* scene) {
    // process each mesh located at the current node
    for(unsigned int i = 0; i < node->mNumMeshes; i++) {
        // the node object only contains indices to index the actual objects in the scene.
        // the scene contains all the data, node is there to keep things organized (like relations between nodes).
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        Mesh mesh_obj = process_mesh(mesh, scene);
        
        m_meshes.push_back(mesh_obj);
        
        m_num_vertices += mesh_obj.m_vertices.size(); // keep count of total model vertices
        m_num_indices += mesh_obj.m_indices.size(); // keep count of total model indices
    }

    // after we've processed all of the meshes we recursively process each of the children nodes
    for(unsigned int i = 0; i < node->mNumChildren; i++)
        process_node(node->mChildren[i], scene);
}

Mesh ModelProcessor::process_mesh(aiMesh* mesh, const aiScene* scene) {
    std::vector<MeshVertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<MeshTexture> textures;

    // walk through each of the mesh's vertices
    for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
        MeshVertex vertex;

        // positions
        copy_assimp_vec3(vertex.position, mesh->mVertices[i]);

        // normals
        if(mesh->HasNormals())
            copy_assimp_vec3(vertex.normal, mesh->mNormals[i]);

        // texture coordinates
        if(mesh->mTextureCoords[0]) {
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vertex.tex_coords.x = mesh->mTextureCoords[0][i].x; 
            vertex.tex_coords.y = mesh->mTextureCoords[0][i].y;
        
            // tangent
            copy_assimp_vec3(vertex.tangent, mesh->mTangents[i]);
            
            // bitangent
            copy_assimp_vec3(vertex.bitangent, mesh->mBitangents[i]);
        } else {
            vertex.tex_coords = glm::vec2(0.0f);
        }

        vertices.push_back(vertex);
    }

    // now walk through each of the mesh's faces and retrieve the corresponding vertex indices.
    // (we have used: aiProcess_Triangulate)
    for(unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        
        // retrieve all indices of the face and store them in the indices vector
        std::copy(face.mIndices, face.mIndices + face.mNumIndices, std::back_inserter(indices));
    }

    // process materials
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    
    // diffuse maps
    std::vector<MeshTexture> diffuse_maps = load_material_textures(material, aiTextureType_DIFFUSE, MeshTextureType::DIFFUSE);
    textures.insert(textures.end(), diffuse_maps.begin(), diffuse_maps.end());
    
    // specular maps
    std::vector<MeshTexture> specular_maps = load_material_textures(material, aiTextureType_SPECULAR, MeshTextureType::SPECULAR);
    textures.insert(textures.end(), specular_maps.begin(), specular_maps.end());
    
    // normal maps
    // assimp: "The texture is a (tangent space) normal-map"
    std::vector<MeshTexture> normal_maps = load_material_textures(material, aiTextureType_NORMALS, MeshTextureType::NORMAL);
    textures.insert(textures.end(), normal_maps.begin(), normal_maps.end());

    // TBD: USE HEIGHT MAPS AS MeshTextureType::NORMAL FOR NOW. Fix height maps to parallax mapping?
    std::vector<MeshTexture> height_maps = load_material_textures(material, aiTextureType_HEIGHT, MeshTextureType::NORMAL);
    textures.insert(textures.end(), height_maps.begin(), height_maps.end());
    
    // ambient maps
    std::vector<MeshTexture> ambient_maps = load_material_textures(material, aiTextureType_AMBIENT, MeshTextureType::AMBIENT);
    textures.insert(textures.end(), ambient_maps.begin(), ambient_maps.end());
    
    /// DEBUGGING
    // height maps 
    // assimp: "The texture is a height map."
    // std::vector<MeshTexture> normal_maps = load_material_textures(material, aiTextureType_NORMALS, MeshTextureType::HEIGHT);
    // textures.insert(textures.end(), normal_maps.begin(), normal_maps.end());
    
    // for(unsigned int i = 0; i < material->GetTextureCount(AI_MATKEY_METALLIC_TEXTURE); i++) {
        // aiString str;
        // material->GetTexture(AI_MATKEY_METALLIC_TEXTURE, &str); // aiTextureType_METALNESS
        // std::cout << "metallic: " << str.C_Str() << '\n';

        // material->GetTexture(AI_MATKEY_ROUGHNESS_TEXTURE, &str); // aiTextureType_DIFFUSE_ROUGHNESS
        // std::cout << "roughness: " << str.C_Str() << '\n';

        // material->GetTexture(aiTextureType_UNKNOWN, 0, &str);
        // std::cout << "unknown: " << str.C_Str() << '\n';

        // material->GetTexture(AI_MATKEY_SHEEN_COLOR_TEXTURE, &str);
        // std::cout << "unknown: " << str.C_Str() << '\n';
    // }
    

    // enum textures { bool specular; }
    // each model can have multiple textures of the same type. ex: door normals, window normals, floor normals etc.

    return Mesh(std::move(vertices), std::move(indices), std::move(textures));
}

// checks all material textures of a given type and loads the textures if they're not loaded yet.
std::vector<MeshTexture> ModelProcessor::load_material_textures(aiMaterial* mat, aiTextureType assimp_tex_type, MeshTextureType mesh_tex_type)
{
    std::vector<MeshTexture> textures;

    for(unsigned int i = 0; i < mat->GetTextureCount(assimp_tex_type); i++) {
        aiString str;
        mat->GetTexture(assimp_tex_type, i, &str);

        // check if texture was loaded before. If so, continue to next iteration: skip loading a new texture
        bool skip = false;
        for(unsigned int j = 0; j < m_textures_loaded.size(); j++) {
            if(std::strcmp(m_textures_loaded[j].path.data(), str.C_Str()) == 0) {
                // a texture with the same filepath has already been loaded, continue to next one
                textures.push_back(m_textures_loaded[j]);
                skip = true;

                break;
            }
        }

        // if texture hasn't been loaded already, load it
        if(!skip) {   
            MeshTexture texture;
            texture.type = mesh_tex_type;
            texture.path = str.C_Str();
            
            textures.push_back(texture);
            
            // store it as texture loaded for entire model,
            // to ensure we won't unnecessary load duplicate textures.
            m_textures_loaded.push_back(texture);
        }
    }

    return textures;
}

void ModelProcessor::clear() {
    m_num_vertices = 0;
    m_num_indices = 0;

    m_textures_loaded.clear();
    m_meshes.clear();
    m_model_path.clear();
    m_gamma_correction = ENABLE_GAMMA_CORRECTION;
}