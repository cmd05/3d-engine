#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} vs_out;

// ---

// #define NR_POINT_LIGHTS 1

// uniform vec3 viewPos;

// struct Light {
//     vec3 position;
//     vec3 color;
// };

// out VS_OUT {
//     vec3 TangentFragPos;
//     vec2 TexCoords;
//     vec3 TangentViewPos;
//     uniform Light TangentSpace_point_lights[NR_POINT_LIGHTS];
// } vs_out;

// ---

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    // vs_out.Normal = aNormal;

    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vs_out.Normal = normalize(normalMatrix * aNormal);

    vs_out.TexCoords = aTexCoords;

    gl_Position = projection * view * model * vec4(aPos, 1.0);
}