#version 330 core
out vec4 FragColor;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;

uniform bool ambient_tex_exists;
uniform bool diffuse_tex_exists;
uniform bool specular_tex_exists;


uniform float u_ambient_strength;

// ---

#define NR_POINT_LIGHTS 1

struct Light {
    vec3 position;
    vec3 color;
};

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;

    vec3 TangentFragPos;
    vec3 TangentViewPos;
    Light TangentPointLights[NR_POINT_LIGHTS];
} fs_in;

in vec3 world_Normal;

// ---

const float att_const = 1.0;
const float att_linear = 0.00009;
const float att_quadratic = 0.000032;

bool normalMapping = true;

void main() {
    vec3 normal = world_Normal;

    if(normalMapping) {
        normal = texture(texture_normal1, fs_in.TexCoords).rgb;
        normal = normalize(normal * 2.0 - 1.0);
    }

    // diffuse color
    vec3 color = texture(texture_diffuse1, fs_in.TexCoords).rgb;

    // ambient
    vec3 ambient = u_ambient_strength * color;

    // specular
    // vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);

    vec3 result = vec3(0.0);

    for(int i = 0; i < NR_POINT_LIGHTS; i++) {
        vec3 lightDir = normalize(fs_in.TangentPointLights[i].position - fs_in.TangentFragPos);
        
        float diff = max(dot(lightDir, normal), 0.0);
        vec3 diffuse = diff * color;

        // vec3 halfwayDir = normalize(lightDir + viewDir);
        // float spec = pow(max(dot(normal, halfwayDir), 0.0), 256.0);
        // vec3 specular = vec3(0.3) * spec;

        // // orange specular => no specular texture for model/mesh
        // if(specular_tex_exists)
        //     specular *= vec3(texture(texture_specular1, fs_in.TexCoords));
        // else
        //     specular = vec3(0.0);
        vec3 specular = vec3(0.0);

        float dist = length(fs_in.TangentPointLights[i].position - fs_in.TangentFragPos);
        float attenuation = 1.0 / (att_const + att_linear * dist + att_quadratic * (dist * dist));

        // note: attenuation won't apply to ambient component
        // diffuse *= attenuation;
        // specular *= attenuation;

        vec3 component = (ambient + diffuse + specular) * fs_in.TangentPointLights[i].color;

        result += component;
    }

    result = pow(result, vec3(1.0/2.2)); // do gamma correction
    FragColor = vec4(vec3(result), 1.0);
}