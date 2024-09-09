#pragma once

#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>

class Shader
{
public:
    Shader(std::string const& vertex_path, std::string const& fragment_path);

    void activate();

    template<typename T>
    void set_uniform(const std::string& name, const T& value);

    GLuint get_id() const;
private:
    GLuint m_id;
};

template<typename T>
void Shader::set_uniform(const std::string& name, const T& value)
{
    if constexpr (std::is_same_v<T, glm::mat4>)
    {
        glUniformMatrix4fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, &value[0][0]);
    }
    if constexpr (std::is_same_v<T, glm::vec3>)
    {
        glUniform3fv(glGetUniformLocation(m_id, name.c_str()), 1, &value[0]); 
    }
    
    // use with old math lib
    // if constexpr (std::is_same_v<T, Mat44>)
    // {
    //     glUniformMatrix4fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_TRUE, (GLfloat*)value.m);
    // }
    // if constexpr (std::is_same_v<T, Vec3>)
    // {
    //     glUniform3fv(glGetUniformLocation(m_id, name.c_str()), 1, (GLfloat*)&value);
    // }
}