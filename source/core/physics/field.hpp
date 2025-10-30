#ifndef MYPROJECT_FIELD_HPP
#define MYPROJECT_FIELD_HPP
#include <vector>
#include <glm/glm.hpp>

struct Field
{
  int width, height, depth;
  std::vector<glm::vec3> forces;

  Field(int w, int h, int d) : width(w), height(h), depth(d)
  {
    forces.resize(w * h * d, glm::vec3(0.0f));
  }

  // 단순 예: 중력 + 원점 당기는 힘
  glm::vec3 sample(glm::vec3 pos)
  {
    glm::vec3 gravity(0, -9.8f, 0);
    glm::vec3 centerPull = -0.1f * pos; // 원점으로 끌기
    return gravity + centerPull;
  }

  void applyToParticles(std::vector<glm::vec3>& positions,
                        std::vector<glm::vec3>& velocities,
                        float dt)
  {
    for (size_t i = 0; i < positions.size(); i++)
    {
      glm::vec3 f = sample(positions[i]);
      velocities[i] += f * dt;
      positions[i] += velocities[i] * dt;
    }
  }
};


#endif //MYPROJECT_FIELD_HPP
