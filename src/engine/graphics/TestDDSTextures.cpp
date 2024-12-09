#include <glad/glad.h> 
#include <stb/stb_image.h>

#include <array>
#include <unordered_map>
#include <string>

#include <engine/graphics/TextureManager.hpp>
#include <engine/graphics/Shader.hpp>
#include <engine/graphics/objects/GraphicsObjects.hpp>

#include <lib/utilities/DebugAssert.hpp>

#include <fstream>

#define DDSKTX_IMPLEMENT
#include <dds-ktx/dds-ktx.h>
#include <filesystem>
#include <iterator>

TextureManager::TextureManager() {

}



// if same texture path already exists return the texture id
// else create new texture and return its id 
unsigned int TextureManager::texture_from_file(std::string file_path, bool gamma_correction, bool vflip_texture) {
    if(vflip_texture)
        stbi_set_flip_vertically_on_load(true);

    // check if texture exists
    auto it = m_loaded_textures.begin();
    if((it = m_loaded_textures.find(file_path)) != m_loaded_textures.end())
        return it->second;

    // create a new texture
    unsigned int texture_id;
    glGenTextures(1, &texture_id);

    int width, height, num_components;

    // unsigned char *img_data = stbi_load(file_path.c_str(), &width, &height, &num_components, 0);
    
    // check if file at same path with ".tex_bin" exists
    unsigned char* img_data = nullptr;

    // const aiScene* scene = importer.ReadFile(path, ASSIMP_READ_FLAGS);
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, scene->mTextures[ti]->mWidth, scene->mTextures[ti]->mHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE,
    // scene->mTextures[ti]->pcData);

    std::string tex_dir = std::filesystem::path(file_path).parent_path().string();
    std::string tex_fname = std::filesystem::path(file_path).filename().string();
    std::size_t ext_sym = tex_fname.find_last_of("."); 
    std::string raw_name = tex_fname.substr(0, ext_sym); 

    std::string dds_path = tex_dir + "/dds/" + raw_name + ".dds";

    std::ifstream ifs_dds {dds_path, std::ios_base::binary};

    if(!ifs_dds) assert("fail");

    // int fsize = std::filesystem::file_size(dds_path);

    std::vector<unsigned char> v_dds;

    // Stop eating new lines in binary mode!!!
    ifs_dds.unsetf(std::ios::skipws);

    // get its size:
    std::streampos fileSize;

    ifs_dds.seekg(0, std::ios::end);
    fileSize = ifs_dds.tellg();
    ifs_dds.seekg(0, std::ios::beg);

    // reserve capacity
    v_dds.reserve(fileSize);

    // read the data:
    v_dds.insert(v_dds.begin(),
               std::istream_iterator<unsigned char>(ifs_dds),
               std::istream_iterator<unsigned char>());

    // std::copy(std::istreambuf_iterator<unsigned char>(ifs_dds), 
    //             std::istreambuf_iterator<unsigned char>(),
    //             std::back_inserter(v_dds));

    ddsktx_texture_info tc = {0};

    GLenum internal_format;


    if(ddsktx_parse(&tc, (void*) v_dds.data(), v_dds.size(), NULL)) {
        assert(tc.depth == 1);
        assert(tc.num_layers == 1);

        
    if (tc.format == DDSKTX_FORMAT_BC1) {
        internal_format = gamma_correction ? GL_COMPRESSED_SRGB_S3TC_DXT1_EXT : GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
        // internal_format = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
    } else if (tc.format == DDSKTX_FORMAT_BC3) {
        // if(flags & DDSKTX_TEXTURE_FLAG_ALPHA) // assume always has alpha
     
        internal_format = gamma_correction ? GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT : GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
        // internal_format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
    }

        // DDSKTX_TEXTURE_FLAG_ALPHA
        // yes it is compressed
        // ddsktx_get_sub(&tc, &sub_data, dds_data, size, 0, 0, mip);

        // glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_id);
        // glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, data_format, GL_UNSIGNED_BYTE, img_data);
        
        for (int mip = 0; mip < tc.num_mips; mip++) {
            ddsktx_sub_data sub_data;
            ddsktx_get_sub(&tc, &sub_data, (void*) (v_dds.data()), v_dds.size(), 0, 0, mip);
            // Fill/Set texture sub resource data (mips in this case)
            if (ddsktx_format_compressed(tc.format)) {
                glCompressedTexImage2D(GL_TEXTURE_2D, mip, internal_format, sub_data.width, sub_data.height, 0, sub_data.size_bytes, sub_data.buff);
            } else {
                assert("fail");
            }
                // glCompressedTexImage2D(..);
        }

        // glCompressedTexImage2D(GL_TEXTURE_2D, 0, internal_format, tc.width, tc.height, 0, tc.size_bytes, v_dds.data() + tc.data_offset);
        // glGenerateMipmap(GL_TEXTURE_2D);

        // GLenum err = glGetError();
        // if (err != GL_NO_ERROR) {
        //     std::cerr << "OpenGL Error: " << err << std::endl;
        // }


        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else {
        assert("FAILED TO PARSE DDS");
    }

    // free(ddsdata);

    // if (dxt1) { // DDSKTX_FORMAT_BC1 => no alpha, no srgb, 
    //     internal_format = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
    // } else if (dxt5) { // has alpha, internal_format decided based on `gamma_correction`
    //     // if(flags & DDSKTX_TEXTURE_FLAG_ALPHA) // assume always has alpha
    //         internal_format = gamma_correction ? GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT : GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
    // }
    
    //     Accepted by the <internalformat> parameter of TexImage2D,
    // CopyTexImage2D, and CompressedTexImage2DARB and the <format> parameter
    // of CompressedTexSubImage2DARB:

    //     COMPRESSED_SRGB_S3TC_DXT1_EXT                  0x8C4C
    //     COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT            0x8C4D
    //     COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT            0x8C4E
        // COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT            0x8C4F

        // case Format::BC1_RGB_UNORM:      return GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
    // case Format::BC1_RGBA_UNORM:     return GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
    // case Format::BC1_RGB_SRGB:       return GL_COMPRESSED_SRGB_S3TC_DXT1_EXT;
    // case Format::BC1_RGBA_SRGB:      return GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT;
    // case Format::BC2_RGBA_UNORM:     return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
    // case Format::BC2_RGBA_SRGB:      return GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT;
    // case Format::BC3_RGBA_UNORM:     return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
    // case Format::BC3_RGBA_SRGB:      return GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT;
    // case Format::BC4_R_UNORM:        return GL_COMPRESSED_RED_RGTC1;
    // case Format::BC4_R_SNORM:        return GL_COMPRESSED_SIGNED_RED_RGTC1;
    // case Format::BC5_RG_UNORM:       return GL_COMPRESSED_RG_RGTC2;
    // case Format::BC5_RG_SNORM:       return GL_COMPRESSED_SIGNED_RG_RGTC2;
    // case Format::BC6H_RGB_UFLOAT:    return GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT;
    // case Format::BC6H_RGB_SFLOAT:    return GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT;
    // case Format::BC7_RGBA_UNORM:     return GL_COMPRESSED_RGBA_BPTC_UNORM;
    // case Format::BC7_RGBA_SRGB:      return GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM;
    

    // std::string bin_path = file_path + ".tex_bin";
    // bool bin_exists = (std::ifstream{bin_path, std::ios::binary}).good();

    // // stbi_load: 8-bits-per-channel interface
    // if(!bin_exists)
    //     dump_texture_bin(file_path, bin_path, img_data, width, height, num_components);
    // else
    //     load_texture_bin(bin_path, img_data, width, height, num_components);

    // if(img_data) {
    //     GLenum internal_format, data_format;

    //     if (num_components == 1) {
    //         internal_format = GL_RED;
    //         data_format = GL_RED;
    //     } else if (num_components == 3) {
    //         internal_format = gamma_correction ? GL_SRGB : GL_RGB;
    //         data_format = GL_RGB;
    //     } else if (num_components == 4) {
    //         internal_format = gamma_correction ? GL_SRGB_ALPHA : GL_RGBA;
    //         data_format = GL_RGBA;
    //     }

    //     glBindTexture(GL_TEXTURE_2D, texture_id);
    //     glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, data_format, GL_UNSIGNED_BYTE, img_data);
    //     glGenerateMipmap(GL_TEXTURE_2D);

    //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // } else {
    //     ASSERT_MESSAGE("Texture data could not be loaded: " << file_path);
    // }

    // // stbi_image_free(img_data);

    // if(!bin_exists)
    //     stbi_image_free(img_data);
    // else
    //     free_tex_bytes(img_data);

    glBindTexture(GL_TEXTURE_2D, 0); // reset bound texture
    m_loaded_textures[file_path] = texture_id; // add to loaded textures

    return texture_id;
}

void TextureManager::load_texture_bin(std::string bin_path, unsigned char*& img_data, int& width, int& height, int& num_components) {
    std::ifstream ifs_bin {bin_path, std::ios::binary};
    
    // 1. read width
    ifs_bin.read(reinterpret_cast<byte_ptr>(&width), sizeof(width));

    // 2. read height
    ifs_bin.read(reinterpret_cast<byte_ptr>(&height), sizeof(height));

    // 3. read number of components
    ifs_bin.read(reinterpret_cast<byte_ptr>(&num_components), sizeof(num_components));

    std::size_t num_bytes = width * height * num_components;

    // 4. read image data
    alloc_tex_bytes(img_data, num_bytes);
    ifs_bin.read(reinterpret_cast<byte_ptr>(img_data), num_bytes * sizeof(unsigned char));
}

void TextureManager::dump_texture_bin(std::string tex_path, std::string bin_path, unsigned char*& img_data, int& width, int& height, int& num_components) {
    // create binary file
    std::ofstream ofs_bin{bin_path, std::ios::binary};
    img_data = stbi_load(tex_path.c_str(), &width, &height, &num_components, 0);

    // stbi_load: 8-bits-per-channel interface
    std::size_t num_bytes = width * height * num_components;

    // 1. dump width
    ofs_bin.write(reinterpret_cast<byte_ptr>(&width), sizeof(width));

    // 2. dump height
    ofs_bin.write(reinterpret_cast<byte_ptr>(&height), sizeof(height));

    // 3. dump number of components
    ofs_bin.write(reinterpret_cast<byte_ptr>(&num_components), sizeof(num_components));
    
    // 4. dump image data
    ofs_bin.write(reinterpret_cast<byte_ptr>(img_data), num_bytes * sizeof(unsigned char));
}

void TextureManager::alloc_tex_bytes(unsigned char*& img_data, int num_bytes) {
    img_data = new unsigned char[num_bytes];
}

void TextureManager::free_tex_bytes(unsigned char*& img_data) {
    delete[] img_data;
}

cubemaps_interface_type TextureManager::load_cubemaps(std::unordered_map<std::string, CubemapFaces> cubemaps) {
    cubemaps_interface_type loaded_cubemaps;

    for(auto [name, cubemap] : cubemaps)
        loaded_cubemaps[name] = add_cubemap(cubemap);

    return loaded_cubemaps;
}

unsigned int TextureManager::add_cubemap(CubemapFaces faces) {
    unsigned int texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

    // order as required by GL_TEXTURE_CUBE_MAP_POSITIVE_X
    cubemap_array<std::string> faces_ordered = {faces.right, faces.left, faces.top, faces.bottom, faces.front, faces.back};

    int width, height, num_components;

    for(std::size_t i = 0; i < faces_ordered.size(); i++) {
        // TODO: do same binary loading as texture_from_file() for add_cubemap()
        unsigned char* img_data = stbi_load(faces_ordered[i].c_str(), &width, &height, &num_components, 0);

        if (img_data)
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img_data);
        else
            ASSERT_MESSAGE("Cubemap texture failed to load at path: " << faces_ordered[i]);

        stbi_image_free(img_data);
    }

    // set cubemap properties
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    m_cubemaps.push_back(texture_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return texture_id;
}

void TextureManager::draw_cubemap(unsigned int cubemap_id, const std::unique_ptr<Shader>& cubemap_shader, const CameraWrapper& camera_wrapper) {
    glDepthFunc(GL_LEQUAL); // change depth function so depth test passes when values are equal to depth buffer's content

    cubemap_shader->activate();

    // TBD: fix setting texture units and other constant uniforms before render loop (in constructor / rendersystem::init())
    // this can be fixed by having shaders specific to each class. i.e cubemap_shader is initialized in TextureManager
    // we can have a ResourceManager to hold all the shader objects in memory and return pointers (and an id) to them
    cubemap_shader->set_uniform<int>("skybox", 0);

    glm::mat4 view = glm::mat4(glm::mat3(camera_wrapper.get_view_matrix())); // remove translation from the view matrix
    glm::mat4 projection = camera_wrapper.get_projection_matrix();
    glm::mat4 vp_matrix = projection * view;

    cubemap_shader->set_uniform<glm::mat4>("u_VP_matrix", vp_matrix);

    glBindVertexArray(g_graphics_objects.cube.VAO);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_id);

    // draw cubemap
    glDrawArrays(GL_TRIANGLES, 0, g_graphics_objects.cube.num_vertices);

    glBindVertexArray(0);
    glDepthFunc(GL_LESS); // set depth function back to default
}

bool TextureManager::gamma_correct_required(MeshTextureType tex_type) const {
    // return `true` if texture type is not in `m_no_gamma_correct`
    return !(GraphicsConfig::NO_GAMMA_CORRECT_MAPS & tex_type);
}