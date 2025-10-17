#version 450

layout(location = 0) in vec3 fragPosition;

layout(set = 0, binding = 0) uniform sampler2DShadow shadowMap;
layout(set = 0, binding = 1) uniform Light
{
    mat4 lightViewProj;
} lightUBO;

float ShadowFactor(vec3 worldPos)
{
    vec4 lightSpacePos = lightUBO.lightViewProj * vec4(worldPos, 1.0);
    // [-1,1] → [0,1] 변환 (Vulkan/GL convention)
    vec3 projCoords = lightSpacePos.xyz / lightSpacePos.w * 0.5 + 0.5;
    // Shadow lookup (1.0 = lit, 0.0 = in shadow)
    return texture(shadowMap, projCoords);
}
