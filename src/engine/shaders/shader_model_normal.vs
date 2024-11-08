#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

// ---

// TODO: put this in common glsl includes
// eventually move to ssbo for sending number of lights through uniform
#define NR_POINT_LIGHTS 1

struct Light {
    vec3 position;
    vec3 color;
};

uniform vec3 viewPos;
uniform Light pointLights[NR_POINT_LIGHTS];

out VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;

    vec3 TangentFragPos;
    vec3 TangentViewPos;
    Light TangentPointLights[NR_POINT_LIGHTS];
} vs_out;

out vec3 world_Normal;

// ---

precision highp float;
uniform mat3 normalMatrix;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 u_MVP;

void main() {
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    vs_out.TexCoords = aTexCoords;
    // mat3 normalMatrix = transpose(inverse(mat3(model)));

    // convert normals from model space to world space
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    
    // re-orthogonalize T with respect to N (Gram-Schmidt process) 
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);

    mat3 inverse_TBN = transpose(mat3(T, B, N)); // inverse TBN matrix: world space to tangent space

    // convert to tangent space
    vs_out.TangentFragPos = inverse_TBN * vs_out.FragPos;
    vs_out.TangentViewPos = inverse_TBN * viewPos;
    
    for(int i = 0; i < NR_POINT_LIGHTS; i++) {
        vs_out.TangentPointLights[i].position = inverse_TBN * pointLights[i].position;
        vs_out.TangentPointLights[i].color = pointLights[i].color;
    }

    world_Normal = normalize(normalMatrix * aNormal);

    gl_Position = u_MVP * vec4(aPos, 1.0);
}