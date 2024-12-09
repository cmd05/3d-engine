#version 460 core

out vec4 FragColor;

#include "include/config.glsl"
#include "include/data_types.glsl"
#include "include/ub_gui_state.glsl"

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;

uniform MeshTexturesAvailable u_mesh_textures_available;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;

    vec3 TangentFragPos;
    vec3 TangentViewPos;
    Light TangentPointLights[NR_POINT_LIGHTS];

    vec3 world_normal;
} fs_in;

const float att_const = 1.0;
const float att_linear = 0.00009;
const float att_quadratic = 0.000032;
bool normalMapping = true;

void main() {
    vec3 normal = fs_in.world_normal; // use world_normal by default

    if(normalMapping && u_mesh_textures_available.normal != 0) {
        normal = texture(texture_normal1, fs_in.TexCoords).rgb;
        normal = normalize(normal * 2.0 - 1.0);
    }

    // diffuse color
    vec3 color = texture(texture_diffuse1, fs_in.TexCoords).rgb;

    // ambient
    vec3 ambient = u_ambient_strength * color;

    // specular
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);

    vec3 result = vec3(0.0);

    for(int i = 0; i < NR_POINT_LIGHTS; i++) {
        vec3 lightDir = normalize(fs_in.TangentPointLights[i].position - fs_in.TangentFragPos);
        
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
            float dist = length(fs_in.TangentPointLights[i].position - fs_in.TangentFragPos);
            float attenuation = 1.0 / (att_const + att_linear * dist + att_quadratic * (dist * dist));

            // note: attenuation won't apply to ambient component
            diffuse *= attenuation;
            specular *= attenuation;
        }

        vec3 component = (ambient + diffuse + specular) * fs_in.TangentPointLights[i].color;

        result += component;
    }

    // always gamma correct the final fragment color for drawing meshes
    // result = pow(result, vec3(1.0 / GAMMA_FACTOR));
    
    FragColor = vec4(vec3(result), 1.0);
}