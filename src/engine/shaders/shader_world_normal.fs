#version 460 core

out vec4 FragColor;

#include "include/config.glsl"
#include "include/data_types.glsl"
#include "include/ub_gui_state.glsl"

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_metallic_roughness1;

uniform MeshTexturesAvailable u_mesh_textures_available;
uniform Light u_point_lights[NR_POINT_LIGHTS];

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 world_normal;
    vec3 view_pos;
    mat3 TBN_matrix;

    // vec3 TangentFragPos;
    // vec3 TangentViewPos;
    // Light TangentPointLights[NR_POINT_LIGHTS];
} fs_in;

const float att_const = 1.0;
const float att_linear = 0.00009;
const float att_quadratic = 0.000032;
bool normalMapping = true;

// (G - roughness, B - metallic)
void main() {
    vec3 normal = fs_in.world_normal; // use world_normal by default

    if(normalMapping && u_mesh_textures_available.normal != 0) {
        normal = texture(texture_normal1, fs_in.TexCoords).rgb;
        normal = normalize(normal * 2.0 - 1.0);
        normal = fs_in.TBN_matrix * normal; // convert tangent space normal to world normal
    }

    vec3 color = texture(texture_diffuse1, fs_in.TexCoords).rgb;
    vec3 ambient = u_ambient_strength * color;
    vec3 viewDir = normalize(fs_in.view_pos - fs_in.FragPos);

    vec3 result = vec3(0.0);

    for(int i = 0; i < NR_POINT_LIGHTS; i++) {
        vec3 lightDir = normalize(u_point_lights[i].position - fs_in.FragPos);
        
        float diff = max(dot(lightDir, normal), 0.0);
        vec3 diffuse = diff * color;
        vec3 specular = vec3(0.0);
    
        // debugging: orange specular => no specular texture for model/mesh
        if(u_mesh_textures_available.specular != 0) {
            vec3 halfwayDir = normalize(lightDir + viewDir);
            float spec = pow(max(dot(normal, halfwayDir), 0.0), 256.0);
            vec3 specular = vec3(0.3) * spec;
            specular *= vec3(texture(texture_specular1, fs_in.TexCoords));
        }

        if(u_attenuation) {
            float dist = length(u_point_lights[i].position - fs_in.FragPos);
            float attenuation = 1.0 / (att_const + att_linear * dist + att_quadratic * (dist * dist));

            // note: attenuation won't apply to ambient component
            diffuse *= attenuation;
            specular *= attenuation;
        }

        vec3 component = (ambient + diffuse + specular) * u_point_lights[i].color;
        result += component;
    }

    FragColor = vec4(vec3(result), 1.0);
}