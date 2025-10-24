#version 450
#extension GL_EXT_nonuniform_qualifier: require

layout (location = 0) in vec2 uv;
layout (location = 0) out vec4 outColor;

layout (set = 0, binding = 2) uniform sampler2D bindlessTexture[];

layout(push_constant) uniform PushData {
    int gBufferPositon;
    int gBufferNormal;
    int gBufferAlbedo;
    int gBufferRoughness;
    int depthBuffer;
    int shadowBuffer;
    int lightningBuffer;
    int postProcessBuffer;
} pc;

void main() {
    float d = texture(bindlessTexture[pc.depthBuffer], uv).r;
    float dep = d*10;
    float z = clamp(pow(dep, 0.5), 0.0, 1.0);
    outColor = vec4(1-z, 1- z, 1-z, 1.0);
}
