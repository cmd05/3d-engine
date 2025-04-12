#version 460 core

out vec4 FragColor;

#include "include/config.glsl"
#include "include/data_types.glsl"
#include "include/ub_gui_state.glsl"
#include "include/pbr_functions.glsl"

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_metallic_roughness1; // (G - roughness, B - metallic)
uniform sampler2D texture_emissive1;
uniform sampler2D texture_ambient_occlusion1;

uniform MeshTexturesAvailable u_mesh_textures_available;
uniform DirLight u_dir_lights[NR_DIR_LIGHTS];

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 world_normal;
    vec3 view_pos;
    vec3 world_pos;

    mat3 TBN_matrix;
} fs_in;

const float att_const = 1.0;
const float att_linear = 0.00009;
const float att_quadratic = 0.000032;
bool normalMapping = true;

void main() {
    vec3 N = fs_in.world_normal; // use world_normal by default
    if(normalMapping && u_mesh_textures_available.normal != 0) {
        N = texture(texture_normal1, fs_in.TexCoords).rgb;
        N = normalize(N * 2.0 - 1.0);
        N = fs_in.TBN_matrix * N; // convert tangent space normal to world normal
    }

    vec4 albedo_rgba = texture(texture_diffuse1, fs_in.TexCoords).rgba;
    vec3 albedo = albedo_rgba.rgb;
    float diffuse_alpha = albedo_rgba.a;

    vec4 metallic_roughness_rgba = texture(texture_metallic_roughness1, fs_in.TexCoords).rgba;
    float metallic  = metallic_roughness_rgba.b;
    float roughness = metallic_roughness_rgba.g;
    float ao = 1.0;
    vec3 emissive = vec3(0.0);

    if(u_mesh_textures_available.ambient_occlusion != 0)
        ao = texture(texture_ambient_occlusion1, fs_in.TexCoords).r;

    if(u_mesh_textures_available.emissive != 0)
        emissive = texture(texture_emissive1, fs_in.TexCoords).rgb;

    if(albedo_rgba.a < 0.1)
        discard;

    vec3 V = normalize(fs_in.view_pos - fs_in.world_pos); // fragment to view vector (same as W_o)

    // assume that most dielectric surfaces look visually correct with F0=0.04
    // for metallic surfaces, F0 (base reflectivity) will be given by the surface's albedo value
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    // We can think of the loop as solving the integral over Ω for direct light sources.
    // loop over all light sources and add their outgoing radiance contributions
    
    // irradiance (outgoing radiance value)
    vec3 Lo = vec3(0.0); // Lo includes both diffuse and specular contributions

    for(int i = 0; i < NR_DIR_LIGHTS; i++) {
        // first do all lighting calculations in linear space
        vec3 L = normalize(u_dir_lights[i].direction); // frag to light vector (same as W_i)
        vec3 H = normalize(V + L); // halfway vector
        float NdotL = max(dot(N, L), 0.0);

        float cosTheta = NdotL; // both vectors are already normalized
        vec3 radiance = u_dir_lights[i].color * cosTheta; // light color (radiance) when it reaches point p after effect of attenuation

        // calculate cook torrance brdf
        float NDF = DistributionGGX(N, H, roughness); // normal distribution
        float G = GeometrySmith(N, V, L, roughness); // geometry
        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0); // fresnel

        vec3 num = NDF * F * G; // fresnel component is a vec3
        float denom = 4.0 * max(dot(N, V), 0.0) * NdotL + 0.0001; // + 0.0001 to prevent divide by zero
    
        vec3 specular = num / denom; 

        // fresnel value directly corresponds to k_s (ratio of incoming light energy that gets reflected)
        vec3 kS = F; // specular contribution

        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS; // portion that gets refracted

        kD *= 1.0 - metallic; // metallic surfaces don't refract light in PBR, 
                              // so nullify the effect when surface is metallic

        // now implement reflectance equation to get: radiance due to reflection
        // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
        // (kD * (albedo / PI) + specular) is eqv to (diffuse + specular)
        // diffuse is given by (kD * (albedo / PI))
        Lo += (kD * (albedo / PI) + specular) * radiance * NdotL; // add outgoing-radiance due to i-th light source
    }

    // add ambient color to fragment
    vec3 ambient = u_ambient_strength * albedo * ao;

    vec3 color = Lo + ambient + emissive;

    // color = color / (color + vec3(1.0)); // reinhard tone mapping
    // color = pow(color, vec3(1.0 / 2.2)); // gamma correction

    FragColor = vec4(color, 1.0);
}