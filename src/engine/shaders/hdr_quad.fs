#version 330 core

out vec4 FragColor;

in vec2 vs_out_tex_coords;

uniform bool u_debug_depth_buffer;

uniform sampler2D texture_hdr_buffer; // floating point color texture
uniform bool u_hdr_enabled;
uniform float u_exposure;

// const float near_plane = 1.0;
// const float far_plane = 100.0;
// float LinearizeDepth(float depth) {
//     float z = depth * 2.0 - 1.0; // Back to NDC 
//     return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));	
// }

void main() {
    if(u_debug_depth_buffer) {
        float depthValue = texture(texture_hdr_buffer, vs_out_tex_coords).r;
        FragColor = vec4(vec3(depthValue), 1.0);
        // FragColor = vec4(vec3(LinearizeDepth(depthValue) / far_plane), 1.0); // perspective
        return;
    }

    const float gamma = 2.2;
    vec3 hdrColor = texture(texture_hdr_buffer, vs_out_tex_coords).rgb;

    if(u_hdr_enabled) {
        // exposure tone mapping
        vec3 mapped = vec3(1.0) - exp(-hdrColor * u_exposure);
        
        // gamma correction
        mapped = pow(mapped, vec3(1.0 / gamma));
        FragColor = vec4(mapped, 1.0);
    } else {
        // simply pass through value
        // gets clamped between 0.0 and 1.0
        FragColor = vec4(hdrColor, 1.0);
    }
}