#version 450

layout(location = 0) in vec3 inPosition;

layout(set = 0, binding = 0) uniform Light
{
    mat4 lightViewProj;
} lightUBO;

layout(set = 1, binding = 0) uniform Model
{
    mat4 model;
} modelUBO;

void main()
{
    // 월드 좌표 → 라이트 클립 공간
    gl_Position = lightUBO.lightViewProj * modelUBO.model * vec4(inPosition, 1.0);
}


#version 450

layout(location = 0) in vec3 inPosition;

layout(set = 0, binding = 0) uniform Light
{
    mat4 lightView;      // Spot light view matrix
    mat4 lightProj;      // Spot light perspective projection
} lightUBO;

layout(set = 1, binding = 0) uniform Model
{
    mat4 model;
} modelUBO;

void main()
{
    // 월드 좌표 계산
    vec4 worldPos = modelUBO.model * vec4(inPosition, 1.0);

    // Spot light view-projection 변환
    gl_Position = lightUBO.lightProj * lightUBO.lightView * worldPos;
}
