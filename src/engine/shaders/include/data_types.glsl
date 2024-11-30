#define NR_POINT_LIGHTS 1

struct Light {
    vec3 position;
    vec3 color;
};

struct MeshMatrices {
    mat4 model_matrix;
    mat4 mvp_matrix;
    mat3 normal_matrix;
};

// number of textures of each type for mesh
struct MeshTexturesAvailable {
    int ambient;
    int diffuse;
    int specular;
    int normal;
};