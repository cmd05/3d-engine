#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

#define MAX_SKYBOX 10

// reserve: 0,1,2,3 - mvp,model,view,projection matrices
// layout (location = 3) uniform samplerCube u_skybox[MAX_SKYBOX];
// layout (location = 4) uniform int u_selected_skybox;

void main()
{    
    FragColor = texture(skybox, TexCoords);
}