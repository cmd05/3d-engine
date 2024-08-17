#include <glad/glad.h>

#include <graphics/Shader.hpp>

#include <fstream>
#include <iostream>
#include <sstream>

Shader::Shader(std::string const& vertexPath, std::string const& fragmentPath)
{
    std::string fragmentFileContents;
    std::string vertexFileContents;

    // Read in the vertex shader
    std::ifstream vertex_file;

    vertex_file.open(vertexPath);
    std::stringstream vertex_file_stream;
    vertex_file_stream << vertex_file.rdbuf();
    vertex_file.close();
    vertexFileContents = vertex_file_stream.str();

    // Read in the fragment shader
    std::ifstream fragment_file;

    fragment_file.open(fragmentPath);
    std::stringstream fragment_file_stream;
    fragment_file_stream << fragment_file.rdbuf();
    fragment_file.close();
    fragmentFileContents = fragment_file_stream.str();

    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);

    const GLchar* vertex_shader_source = vertexFileContents.c_str();
    glShaderSource(vertex_shader, 1, &vertex_shader_source, nullptr);
    glCompileShader(vertex_shader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(vertex_shader, 512, nullptr, infoLog);
        std::cerr << "Error compiling vertex shader: " << infoLog << "\n";
    }


    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

    const GLchar* fragment_shader_source = fragmentFileContents.c_str();
    glShaderSource(fragment_shader, 1, &fragment_shader_source, nullptr);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(fragment_shader, 512, nullptr, infoLog);
        std::cerr << "Error compiling fragment shader: " << infoLog << "\n";
    }


    mId = glCreateProgram();

    glAttachShader(mId, vertex_shader);
    glAttachShader(mId, fragment_shader);
    glLinkProgram(mId);
    glGetProgramiv(mId, GL_LINK_STATUS, &success);

    if (!success)
    {
        glGetProgramInfoLog(mId, 512, nullptr, infoLog);
        std::cerr << "Error linking shaders: " << infoLog << "\n";
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

void Shader::Activate()
{
    glUseProgram(mId);
}


