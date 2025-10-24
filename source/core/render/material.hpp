//
// Created by dlwog on 25. 10. 23..
//

#ifndef MYPROJECT_MATERIAL_HPP
#define MYPROJECT_MATERIAL_HPP
#include "glm/glm.hpp"

struct MaterialConstant
{
  glm::mat4 modelMatrix = glm::mat4(1.0f);
  glm::vec4 color = glm::vec4(1.0f);
  int albedoTextureIndex = 0;
  int normalTextureIndex = 0;
  int metalicTextureIndex = 0;
  glm::vec3 emissiveColor = glm::vec3(0.0f);
  float metallic = 0.0f;
  float roughness = 0.04f;
  float ao = 1.0f;
  float emission = 0.0f;
  float alphaCutoff = 0.0f;
};
#endif //MYPROJECT_MATERIAL_HPP
