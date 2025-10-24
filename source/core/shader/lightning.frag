#core 450
layout(set=0, binding=0) uniform sampler2D gAlbedo;
layout(set=0, binding=1) uniform sampler2D gNormal;
layout(set=0, binding=2) uniform sampler2D gDepth;

void main() {
    vec3 albedo = texture(gAlbedo, uv).rgb;
    vec3 normal = texture(gNormal, uv).xyz;
    float depth = texture(gDepth, uv).r;

    vec3 color = calculateLighting(albedo, normal, depth);
    outColor = vec4(color, 1.0);
}
