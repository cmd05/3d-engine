#include <fstream>
#include <memory>

#include <glad/glad.h>

#include <engine/graphics/Shader.hpp>
#include <engine/config/ShaderConfig.hpp>

#define STB_INCLUDE_IMPLEMENTATION
#define STB_INCLUDE_LINE_NONE
#include <stb/stb_include.h>

Shader::Shader(const std::string& vertex_path, const std::string& fragment_path, std::string geometry_path) {
    static constexpr std::size_t err_len = 512;
    char stb_err_msg[err_len];

    std::string shader_dir = FS_SHADERS_DIR;
    auto p_vertex_src = std::unique_ptr<char, decltype([](char* ptr) { if(ptr) free(ptr); })>(
            stb_include_file(((std::string) vertex_path).data(), nullptr, shader_dir.data(), stb_err_msg));
    
    auto p_fragment_src = std::unique_ptr<char, decltype([](char* ptr) { if(ptr) free(ptr); })>(
            stb_include_file(((std::string) fragment_path).data(), nullptr, shader_dir.data(), stb_err_msg));

    if (p_vertex_src.get() == nullptr)
        ASSERT_MESSAGE("Error stb_include_file() (SHADER::VERT): " << stb_err_msg);
    
    if (p_fragment_src.get() == nullptr)
        ASSERT_MESSAGE("Error stb_include_file() (SHADER::FRAG): " << stb_err_msg);

    // vertex shader
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);

    const GLchar* vertex_shader_source = p_vertex_src.get();
    glShaderSource(vertex_shader, 1, &vertex_shader_source, nullptr);
    glCompileShader(vertex_shader);

    int success;
    char info_log[err_len];
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(vertex_shader, err_len, nullptr, info_log);
        ASSERT_MESSAGE("Error compiling vertex shader: " << info_log);
    }

    // fragment shader
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

    const GLchar* fragment_shader_source = p_fragment_src.get();
    glShaderSource(fragment_shader, 1, &fragment_shader_source, nullptr);
    glCompileShader(fragment_shader);

    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(fragment_shader, err_len, nullptr, info_log);
        ASSERT_MESSAGE("Error compiling fragment shader: " << info_log);
    }

    // geometry shader
    GLuint geometry_shader = -1;

    if(!geometry_path.empty()) {
        auto p_geometry_src = std::unique_ptr<char, decltype([](char* ptr) { if(ptr) free(ptr); })>(
            stb_include_file(((std::string) geometry_path).data(), nullptr, shader_dir.data(), stb_err_msg));

        if (p_geometry_src.get() == nullptr)
            ASSERT_MESSAGE("Error stb_include_file() (SHADER::GEOM): " << stb_err_msg);
    
        geometry_shader = glCreateShader(GL_GEOMETRY_SHADER);

        const GLchar* geometry_shader_source = p_geometry_src.get();
        glShaderSource(geometry_shader, 1, &geometry_shader_source, nullptr);
        glCompileShader(geometry_shader);
    
        glGetShaderiv(geometry_shader, GL_COMPILE_STATUS, &success);
    
        if (!success) {
            glGetShaderInfoLog(geometry_shader, err_len, nullptr, info_log);
            ASSERT_MESSAGE("Error compiling geometry shader: " << info_log);
        }
    }

    // create program
    m_id = glCreateProgram();

    glAttachShader(m_id, vertex_shader);
    glAttachShader(m_id, fragment_shader);
    if(!geometry_path.empty()) glAttachShader(m_id, geometry_shader);

    glLinkProgram(m_id);
    
    glGetProgramiv(m_id, GL_LINK_STATUS, &success);

    if (!success) {
        glGetProgramInfoLog(m_id, err_len, nullptr, info_log);
        ASSERT_MESSAGE("Error linking shaders: " << info_log);
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    if(!geometry_path.empty()) glDeleteShader(geometry_shader);

    cache_uniform_locations();
}

void Shader::activate() {
    glUseProgram(m_id);
}

GLuint Shader::get_id() const { 
    return m_id; 
}

void Shader::cache_uniform_locations() {
    int count, length, size;
    unsigned int type;
    constexpr auto buf_size = ShaderConfig::MAX_UNIFORM_NAME_LEN;
    char name[buf_size];

    glGetProgramiv(m_id, GL_ACTIVE_UNIFORMS, &count);

    for (int i = 0; i < count; i++) {
        glGetActiveUniform(m_id, (GLuint)i, buf_size, &length, &size, &type, name);
        GLint location = glGetUniformLocation(m_id, name);

        // printf("Uniform #%d Type: %u Name: %s Location: %d Size: %d \n", i, type, name, location, size);
        if(location == -1)
            ENGINE_LOG("Uniform: " << name << " is inactive (type=" << type <<  ") (shader_id=" << m_id << ")");

        UniformData data;
        data.location = location;
        data.type = type;

        m_uniforms_cache[(std::string) name] = data;
    }

    /// Get active attributes
    // glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &count);
    // printf("Active Attributes: %d\n", count);
    // for (i = 0; i < count; i++) {
    //     glGetActiveAttrib(program, (GLuint)i, bufSize, &length, &size, &type, name);
    //     printf("Attribute #%d Type: %u Name: %s\n", i, type, name);
    // }
}