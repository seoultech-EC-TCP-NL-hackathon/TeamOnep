#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;

layout(location = 0) out vec3 fragPosition;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec2 fragUV;

layout(set = 0, binding = 0) uniform Camera
{
    mat4 view;
    mat4 proj;
} camera;

layout(set = 1, binding = 0) uniform Model
{
    mat4 model;
} modelUBO;

void main()
{
    mat4 modelView = camera.view * modelUBO.model;
    fragPosition = vec3(modelUBO.model * vec4(inPosition, 1.0));
    fragNormal = normalize(mat3(transpose(inverse(modelUBO.model))) * inNormal);
    fragUV = inUV;
    gl_Position = camera.proj * camera.view * modelUBO.model * vec4(inPosition, 1.0);
}
