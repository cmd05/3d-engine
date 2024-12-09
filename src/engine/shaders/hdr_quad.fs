#version 330 core

out vec4 FragColor;

in vec2 vs_out_tex_coords;

uniform sampler2D hdrBuffer; // floating point color texture
uniform bool u_hdr_enabled;
uniform float u_exposure;

float map(float value, float min1, float max1, float min2, float max2) {
    return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

float lg(float n, float b) {
    return log(n) / log(b);
}

void main() {
    const float gamma = 2.2;
    vec3 hdrColor = texture(hdrBuffer, vs_out_tex_coords).rgb;

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