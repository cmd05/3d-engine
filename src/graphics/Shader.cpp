#include <fstream>
#include <iostream>
#include <sstream>

#include <glad/glad.h>

#include <graphics/Shader.hpp>


Shader::Shader(std::string const& vertex_path, std::string const& fragment_path)
{
    std::string fragment_file_contents;
    std::string vertex_file_contents;

    // Read in the vertex shader
    std::ifstream vertex_file;

    vertex_file.open(vertex_path);
    std::stringstream vertex_file_stream;

    vertex_file_stream << vertex_file.rdbuf();
    vertex_file_contents = vertex_file_stream.str();

    vertex_file.close();

    // Read in the fragment shader
    std::ifstream fragment_file;

    fragment_file.open(fragment_path);

    std::stringstream fragment_file_stream;
    fragment_file_stream << fragment_file.rdbuf();
    fragment_file_contents = fragment_file_stream.str();

    fragment_file.close();

    // create vertex shader
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);

    const GLchar* vertex_shader_source = vertex_file_contents.c_str();
    glShaderSource(vertex_shader, 1, &vertex_shader_source, nullptr);
    glCompileShader(vertex_shader);

    int success;
    char info_log[512];
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(vertex_shader, 512, nullptr, info_log);
        std::cerr << "Error compiling vertex shader: " << info_log << "\n";
    }


    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

    const GLchar* fragment_shader_source = fragment_file_contents.c_str();
    glShaderSource(fragment_shader, 1, &fragment_shader_source, nullptr);
    glCompileShader(fragment_shader);

    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(fragment_shader, 512, nullptr, info_log);
        std::cerr << "Error compiling fragment shader: " << info_log << "\n";
    }


    m_id = glCreateProgram();

    glAttachShader(m_id, vertex_shader);
    glAttachShader(m_id, fragment_shader);

    glLinkProgram(m_id);
    
    glGetProgramiv(m_id, GL_LINK_STATUS, &success);

    if (!success)
    {
        glGetProgramInfoLog(m_id, 512, nullptr, info_log);
        std::cerr << "Error linking shaders: " << info_log << "\n";
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

void Shader::activate()
{
    glUseProgram(m_id);
}
