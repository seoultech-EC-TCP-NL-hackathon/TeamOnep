#version 450

layout(location = 0) in vec3 fragPosition;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragUV;

layout(location = 0) out vec4 gPosition; // World position
layout(location = 1) out vec4 gNormal;   // Normal (RGB) + optional roughness/metal in A
layout(location = 2) out vec4 gAlbedo;   // Diffuse color

layout(set = 2, binding = 0) uniform sampler2D albedoTexture;

void main()
{
    gPosition = vec4(fragPosition, 1.0);
    gNormal = vec4(normalize(fragNormal) * 0.5 + 0.5, 1.0); // Normal mapped to 0-1
    gAlbedo = texture(albedoTexture, fragUV);
}
