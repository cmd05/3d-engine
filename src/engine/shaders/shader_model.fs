#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

uniform bool ambient_tex_exists;
uniform bool diffuse_tex_exists;
uniform bool specular_tex_exists;
// normal, metallic, roughness ...

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;

struct Light {
    vec3 position;
    vec3 color;
};

#define NR_POINT_LIGHTS 1

uniform Light point_lights[NR_POINT_LIGHTS];

const float att_const = 1.0;
const float att_linear = 0.00009;
const float att_quadratic = 0.000032;

// void main()
// {    
//     FragColor = texture(texture_diffuse1, TexCoords);
//     // FragColor = vec4(vec3(gl_FragDepth), 1.0f);
// }

void main() {
    vec3 color = texture(texture_diffuse1, fs_in.TexCoords).rgb;
    // FragColor = vec4(color, 1.0);

    // // ambient
    vec3 ambient = 0.05 * color;

    // diffuse
    // vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    vec3 normal = normalize(fs_in.Normal);

    // float diff = max(dot(lightDir, normal), 0.0);
    // vec3 diffuse = diff * color;

    // specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    // vec3 reflectDir = reflect(-lightDir, normal);
    // float spec = 0.0;

    // vec3 halfwayDir = normalize(lightDir + viewDir);
    // spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

    // vec3 specular = vec3(0.1) * spec;
    // vec3 result = (ambient + diffuse + specular) * lightColor;

    vec3 result = vec3(0.0);

    for(int i = 0; i < NR_POINT_LIGHTS; i++) {
        vec3 lightDir = normalize(point_lights[i].position - fs_in.FragPos);
        
        float diff = max(dot(lightDir, normal), 0.0);
        vec3 diffuse = diff * color;

        // vec3 reflectDir = reflect(-lightDir, normal);
        
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(normal, halfwayDir), 0.0), 256.0);

        // 0.7, 32.0 for seeing clear specular reflections
        vec3 specular = vec3(0.3) * spec;

        // orange specular => no specular texture for model/mesh
        if(specular_tex_exists)
            specular *= vec3(texture(texture_specular1, fs_in.TexCoords));
        else
            specular = vec3(0.0);
        
        float dist = length(point_lights[i].position - fs_in.FragPos);
        float attenuation = 1.0 / (att_const + att_linear * dist + att_quadratic * (dist * dist));

        // note: attenuation won't apply to ambient component
        diffuse *= attenuation;
        specular *= attenuation;

        vec3 component = (ambient + diffuse + specular) * point_lights[i].color;

        result += component;
    }

    // if(gamma)
    result = pow(result, vec3(1.0/2.2)); // do gamma correction

    FragColor = vec4(vec3(result), 1.0);
}