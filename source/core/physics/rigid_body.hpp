//
// Created by dlwog on 25. 10. 30..
//

#ifndef MYPROJECT_RIGID_BODY_HPP
#define MYPROJECT_RIGID_BODY_HPP
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
struct RigidBody {
  glm::vec3 position;
  glm::vec3 velocity;
  glm::vec3 force;
  float mass;
  glm::mat3 inertia;     // 회전 관성
  glm::quat orientation;
  glm::vec3 angularVelocity;
};
struct Collider {
  enum Type { Sphere, Box, Mesh } type;
  glm::vec3 size;   // Box는 half-size, Sphere는 radius
  int bodyID;       // 어떤 RigidBody에 붙었는지
};

glm::vec3 getForce(glm::vec3 pos) {
  return -k * pos; // 단순 Hooke-like
}
#endif //MYPROJECT_RIGID_BODY_HPP