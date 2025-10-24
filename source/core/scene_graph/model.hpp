#ifndef MODEL_HPP
#define MODEL_HPP
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <../core/scene_graph/mesh.hpp>
#include <../core/scene_graph/material.hpp>
#include "mesh_sub.hpp"
#include "util/transform.h"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include "render/material.hpp"

struct Model
{
  Mesh* mesh = nullptr;
  Material* material = nullptr;
  Submesh submesh{};
  Transform transform;
  MaterialConstant constant{};
  std::string name;
};

#endif
