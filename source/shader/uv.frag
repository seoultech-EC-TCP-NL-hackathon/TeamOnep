#version 450
#extension GL_EXT_nonuniform_qualifier: require
#define PI = 3.141592
struct GPULight {
  vec4 position;
  vec4 direction;
  vec4 color;
  vec2 coneAngles;
  vec2 padding;
};

layout (location = 0) in vec3 fragPos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in mat3 TBN;
layout (location = 6) in vec3 camPosIn;
layout (location = 7) in vec2 uv;
layout (location = 0) out vec4 outColor;

void main() {
  outColor = vec4(fragPos, 1);
}
