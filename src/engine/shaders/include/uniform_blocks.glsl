// TODO: lights as ssbo for dynamic number of lights
// https://www.khronos.org/opengl/wiki/Shader_Storage_Buffer_Object
layout (std140, binding = 2) uniform ub_lights {
    PointLights u_point_lights[NR_POINT_LIGHTS];
};

layout (std140, binding = 3) uniform ub_gui_state {
    float ambient_strength;
    float diffuse_strength;
    float specular_strength;
    float specular_highlight;
};