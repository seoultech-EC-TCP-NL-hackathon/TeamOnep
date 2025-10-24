#ifndef MYPROJECT_SCULPTOR_HPP
#define MYPROJECT_SCULPTOR_HPP

#include <vector>
#include <unordered_map>
#include <cstdint>
#include <cmath>
#include <tuple>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/common.hpp>
#include "brush.hpp"
#include "../scene_graph/mesh_sub.hpp"
#include "../../core/scene_graph/model.hpp"
#include "../../core/scene_graph/mesh.hpp"
#include <algorithm>

#include "dynamic_mesh.hpp"

struct ZBuffer{
  void createBuffer(VkExtent2D extent)
  {
    depth = std::vector<std::vector<float> >(extent.height, std::vector<float>(extent.width, 1.0f));
  }
  std::vector<std::vector<float> > depth;
};

class Painting{};

class Sculptor : Painting{
  friend class Engine;
  friend class UI;
  friend class EventManager;
  friend class SculptorMode;


public:
  Sculptor(Model *model, VkExtent2D extent);
  ~Sculptor() = default;
  bool stroke(glm::vec3 strokeDir);
  bool RayIntersectTriangle(const glm::vec3 &dir,
                            const glm::vec3 &v0,
                            const glm::vec3 &v1,
                            const glm::vec3 &v2,
                            float &t,
                            float &u,
                            float &v);
  void createZBuffer(glm::mat4 proj, glm::mat4 view);
  void rotate(float dYawDeg, float dPitDeg);
  bool castRayToMesh(const glm::vec3 &rayDir);
  void subdivideMesh();
  void subdividePoint(const std::vector<uint32_t> &indices);
  void depthUpdate(int x, int y, float depth);

private:
  std::vector<SculptingPoint> sculptingPoints;
  bool isDynamic     = true;
  bool dirty_        = false;
  bool hitAny        = false;
  float hit_t        = FLT_MAX;
  int hitTriangleIdx = -1;
  ZBuffer zBuffer;
  Brush *brush       = &basicBrush_;
  VkExtent2D extent_;
  GrabBrush grabBrush_;
  InflateBrush inflateBrush_;
  SmoothBrush smoothBrush_;
  StandardBrush basicBrush_;
  DynaMesh dynamicMesh_;
  Model *model_;
};

///
///        ImGui::Separator();
// if (sculpting != nullptr)
// {
//   ImGui::Text("Sculpting Brush");
//   ImGui::SliderFloat("Strength ", &sculpting->sculptor->brush->strength, -1, 1);
//   if (ImGui::Button("Standard:"))
//   {
//     sculpting->sculptor->brush = &sculpting->sculptor->basicBrush_;
//   }
//   if (ImGui::Button("Smooth :"))
//   {
//     sculpting->sculptor->brush = &sculpting->sculptor->smoothBrush_;
//   }
//
//   if (ImGui::Button("Grab : "))
//   {
//     sculpting->sculptor->brush = &sculpting->sculptor->grabBrush_;
//   }
//
//   if (ImGui::Button("inflate: "))
//   {
//     sculpting->sculptor->brush = &sculpting->sculptor->inflateBrush_;
//   }
// }
///
///
///
///
///
///
///
///

#endif //MYPROJECT_SCULPTOR_HPP