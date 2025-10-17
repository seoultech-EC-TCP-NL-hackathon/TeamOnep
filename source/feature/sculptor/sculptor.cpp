#include "sculptor.hpp"
#include "spdlog/details/os.h"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include "glm/detail/compute_common.hpp"
#include "spdlog/pattern_formatter.h"
constexpr float EPSILON    = 1e-6f;
constexpr int DEPTH_RADIUS = 30;

Sculptor::Sculptor(Model *model, VkExtent2D extent) : model_(model), extent_(extent), dynamicMesh_(model->mesh) {}

bool Sculptor::stroke(glm::vec3 strokeDir)
{
  spdlog::debug("[Sculptor] gen ray direction: {} {} {}",
                strokeDir.x,
                strokeDir.y,
                strokeDir.z);

  if (castRayToMesh(strokeDir))
  {
    glm::vec3 hitPoint = strokeDir * hit_t;
    dynamicMesh_.subdivideTriange(hitTriangleIdx);
    float rangeX  = brush->radius / extent_.width;
    float rangeY  = brush->radius / extent_.height;
    float ellipse = std::sqrt(rangeX * rangeX + rangeY * rangeY);
    for (auto &v: model_->mesh->vertices)
    {
      float dist = glm::distance(hitPoint, v.position);
      if (dist < brush->radius)
      {
        SculptingPoint p{};
        p.distance      = dist;
        p.ellipse       = ellipse;
        p.currentHeight = glm::length(v.position);
        p.vertex        = &v;
        sculptingPoints.push_back(p);
      }
    }
    float weightedSum = 0.0f;
    float weightSum   = 0.0f;
    for (auto &point: sculptingPoints)
    {
      float w = 1.0f - glm::clamp(point.distance / ellipse, 0.0f, 1.0f);
      weightedSum += point.currentHeight * w;
      weightSum += w;
    }
    float averageHeight = weightedSum / weightSum;
    for (auto p: sculptingPoints)
    {
      p.vertex->position += p.vertex->normal*brush->strength
                            *brush->attenuate(p.distance, p.ellipse)
                             *glm::dot(p.vertex->normal, strokeDir)
                              *1.0f; //(p.currentHeight- averageHeight);

    }
    sculptingPoints.clear();
    model_->mesh->reNomalCompute();
    return true;
  }
  return false;
}

bool Sculptor::RayIntersectTriangle(const glm::vec3 &dir,
                                    const glm::vec3 &v0,
                                    const glm::vec3 &v1,
                                    const glm::vec3 &v2,
                                    float &t,
                                    float &u,
                                    float &v)
{
  glm::vec3 edge1 = v1 - v0;
  glm::vec3 edge2 = v2 - v0;
  glm::vec3 h     = glm::cross(dir, edge2);
  float angle     = glm::dot(edge1, h);
  if (fabs(angle) < EPSILON) return false;
  float f     = 1.0f / angle;
  glm::vec3 s = -v0;

  u = f * glm::dot(s, h);
  if (u < 0.0f or u > 1.0f) return false;
  glm::vec3 q = glm::cross(s, edge1);

  v = f * glm::dot(dir, q);
  if (v < 0.0f or u + v > 1.0f) return false;

  t = f * glm::dot(edge2, q);
  if (t > EPSILON)
    return true;
  return false;
}

bool Sculptor::castRayToMesh(const glm::vec3 &rayDir)
{
  hitAny         = false;
  hit_t          = FLT_MAX;
  hitTriangleIdx = -1;
  for (size_t i = 0; i < model_->mesh->indices.size(); i += 3)
  {
    glm::vec3 v0 = model_->mesh->vertices[model_->mesh->indices[i]].position;
    glm::vec3 v1 = model_->mesh->vertices[model_->mesh->indices[i + 1]].position;
    glm::vec3 v2 = model_->mesh->vertices[model_->mesh->indices[i + 2]].position;
    float t, u, v;
    if (RayIntersectTriangle(rayDir, v0, v1, v2, t, u, v))
    {
      if (t < hit_t)
      {
        hit_t          = t;
        hitTriangleIdx = i / 3;
        hitAny         = true;
      }
    }
  }
  if (hitAny)
  {
    spdlog::debug("[Sculptor] Hit triangle: {} , t: {}", hitTriangleIdx, hit_t);
  } else
  {
    spdlog::debug("[Sculptor] No hit");
  }
  return hitAny;
}

void Sculptor::createZBuffer(glm::mat4 proj, glm::mat4 view)
{
  zBuffer.createBuffer(extent_);
  glm::mat4 projView = proj * view;
  for (int i = 0; i < model_->mesh->vertices.size(); i++)
  {
    glm::vec4 modelPos = glm::vec4(
                                   model_->mesh->vertices[i].position.x,
                                   model_->mesh->vertices[i].position.y,
                                   model_->mesh->vertices[i].position.z,
                                   1.0f);

    glm::vec4 clipPos = projView * modelPos;
    glm::vec3 ndc     = glm::vec3(clipPos) / clipPos.w;
    float xScreen     = (ndc.x * 0.5f + 0.5f) * extent_.width;
    float yScreen     = (1.0f - (ndc.y * 0.5f + 0.5f)) * extent_.height;
    float zScreen     = ndc.z;
    int xInt          = static_cast<int>(std::round(xScreen));
    int yInt          = static_cast<int>(std::round(yScreen));

    xInt = std::clamp(xInt, 0, static_cast<int>(extent_.width) - 1);
    yInt = std::clamp(yInt, 0, static_cast<int>(extent_.height) - 1);

    uint32_t xPixel = static_cast<uint32_t>(xInt);
    uint32_t yPixel = static_cast<uint32_t>(yInt);
    int32_t radius  = DEPTH_RADIUS;

    if (brush->radius > DEPTH_RADIUS)
    {
      radius = brush->radius;
    }
    if (zScreen < zBuffer.depth[yPixel][xPixel])
    {
      for (int dy = -radius; dy <= radius; ++dy)
      {
        for (int dx = -radius; dx <= radius; ++dx)
        {
          int nx                = std::clamp(xInt + dx, 0, static_cast<int>(extent_.width - 1));
          int ny                = std::clamp(yInt + dy, 0, static_cast<int>(extent_.height - 1));
          zBuffer.depth[ny][nx] = (zScreen < zBuffer.depth[ny][nx]) ?
                                    zScreen :
                                    zBuffer.depth[ny][nx];
          //spdlog::trace("ny: {} nx: {} , d :{}", ny, nx, zScreen);
        }
      }
      //spdlog::trace("z buffer Value : {}", zBuffer.depth[yPixel][xPixel]);
      //spdlog::trace("z buffer idx : {} {}", yPixel, xPixel);
    }
  }
}

void Sculptor::depthUpdate(int x, int y, float depth)
{
  int32_t radius = DEPTH_RADIUS;

  if (brush->radius > DEPTH_RADIUS)
  {
    radius = brush->radius;
  }
  if (brush->strength > 0)
  {
    for (int dy = -radius; dy <= radius; ++dy)
    {
      for (int dx = -radius; dx <= radius; ++dx)
      {
        int nx                = std::clamp(x + dx, 0, static_cast<int>(extent_.width - 1));
        int ny                = std::clamp(y + dy, 0, static_cast<int>(extent_.height - 1));
        zBuffer.depth[ny][nx] = (depth < zBuffer.depth[ny][nx]) ?
                                  depth :
                                  zBuffer.depth[ny][nx];
        return;
      }
    }
  }
  for (int dy = -radius; dy <= radius; ++dy)
  {
    for (int dx = -radius; dx <= radius; ++dx)
    {
      int nx                = std::clamp(x + dx, 0, static_cast<int>(extent_.width - 1));
      int ny                = std::clamp(y + dy, 0, static_cast<int>(extent_.height - 1));
      zBuffer.depth[ny][nx] = (depth > zBuffer.depth[ny][nx]) ?
                                depth :
                                zBuffer.depth[ny][nx];
      return;
    }
  }
}
