#pragma once

#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <lib/utilities/DebugAssert.hpp>

class Shader {
public:
    Shader(std::string const& vertex_path, std::string const& fragment_path, std::string geometry_path = "");

    void activate();

    // set_uniform() expects that user activates appropriate shader before call
    template<typename T>
    void set_uniform(const std::string& name, const std::type_identity_t<T>& value);
    
    template<typename T>
    void set_uniform(int location, const std::type_identity_t<T>& value);

    GLuint get_id() const;

    struct UniformData {
        GLuint type;
        GLint location;
    };
private:
    // std::string name; // name for easy identification

    GLuint m_id;
    std::unordered_map<std::string, UniformData> m_uniforms_cache;
    std::unordered_set<std::string> m_inactive_uniforms; // set of inactive / invalid uniforms

private:
    void cache_uniform_locations();
};

template<typename T>
void Shader::set_uniform(int location, const std::type_identity_t<T>& value) {
    if constexpr (std::is_same_v<T, glm::mat4>)
        glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
    
    if constexpr (std::is_same_v<T, glm::mat3>)
        glUniformMatrix3fv(location, 1, GL_FALSE, &value[0][0]);

    if constexpr (std::is_same_v<T, glm::vec3>)
        glUniform3fv(location, 1, &value[0]);
    
    if constexpr (std::is_same_v<T, float>)
        glUniform1f(location, value); 

    if constexpr (std::is_same_v<T, int>)
        glUniform1i(location, value);
}

template<typename T>
void Shader::set_uniform(const std::string& name, const std::type_identity_t<T>& value) {
    auto it1 = m_inactive_uniforms.find(name);

    // cached inactive uniform
    if(it1 != m_inactive_uniforms.end()) return;

    auto it2 = m_uniforms_cache.find(name);

    // inactive uniform found
    if(it2 == m_uniforms_cache.end()) {
        m_inactive_uniforms.insert(name);
        ENGINE_LOG("Setting invalid / inactive uniform: " << name << " (shader_id=" << m_id << ")");
        return;
    }

    set_uniform<T>(it2->second.location, value);
}