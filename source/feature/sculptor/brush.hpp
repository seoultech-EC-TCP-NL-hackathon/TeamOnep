//
// Created by ljh on 25. 9. 18..
//

#ifndef MYPROJECT_BRUSH_HPP
#define MYPROJECT_BRUSH_HPP
#include "../../core/scene_graph/mesh.hpp"
#include "math.h"

enum class BrushType{
  STANDARD,
  INFLATE,
  FLATTEN,
  SMOOTH,
  PINCH
};

struct SculptingPoint{
  float distance;
  float currentHeight;
  float averageHeight;
  float ellipse;
  VertexAll* vertex;
  glm::vec3 direction;
};

struct Brush{
  float radius = 1.0f;
  float strength   = 0.05;
  float deltaMouse = 0;
  BrushType type;
  float attenuate(float distance, float ellipse)
  {
    float x = distance / ellipse;
    if (x > 1.0f) return 0.0f;
    return std::exp(- x/4);
  }

  virtual float brushing(float distance,
                         float currentHeight,
                         float averageHeight,
                         const glm::vec3 &normal,
                         const glm::vec3 &direction,
                         float ellipse) = 0;
};

struct StandardBrush : public Brush{
  virtual float brushing(float distance,
                         float currentHeight,
                         float averageHeight,
                         const glm::vec3 &normal,
                         const glm::vec3 &direction,
                         float ellipse) override
  {
    return strength * attenuate(distance, ellipse) * (averageHeight - currentHeight);
  }
};

struct SmoothBrush : public Brush{
  virtual float brushing(float distance,
                         float currentHeight,
                         float averageHeight,
                         const glm::vec3 &normal,
                         const glm::vec3 &direction,
                         float ellipse) override
  {
    return strength * (averageHeight - currentHeight);
  }
};

struct InflateBrush : public Brush{
  virtual float brushing(float distance,
                         float currentHeight,
                         float averageHeight,
                         const glm::vec3 &normal,
                         const glm::vec3 &direction,
                         float ellipse) override
  {
    return strength * glm::dot(normal, direction) * attenuate(distance, ellipse);
  }
};

struct GrabBrush : public Brush{
  virtual float brushing(float distance,
                         float currentHeight,
                         float averageHeight,
                         const glm::vec3 &normal,
                         const glm::vec3 &direction,
                         float ellipse) override
  {
    return deltaMouse * (strength * attenuate(distance, ellipse));
  }
};

#endif //MYPROJECT_BRUSH_HPP