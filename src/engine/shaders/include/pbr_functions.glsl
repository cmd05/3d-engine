// Source: https://learnopengl.com/PBR/Theory

const float PI = 3.14159265359;

/**
 * @param cosTheta dot product h.v
 * @param F0 surface reflection at zero incidence 
 */
vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// normal distribution function
float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness; // we have taken alpha as the square of roughness
    float a2 = a * a;

    float NdotH2 = pow(max(dot(N, H), 0.0), 2.0);
    float denom = PI * pow((NdotH2) * (a2 - 1.0) + 1.0, 2);
    float num = a2;

    return num / denom;
}

// geometry function
float GeometrySchlickGGX(float NdotV, float roughness) {
    float a = roughness + 1.0;
    float k = (a * a) / 8.0;

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}

// geometry smith function
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);

    float ggx1 = GeometrySchlickGGX(NdotV, roughness);
    float ggx2 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}