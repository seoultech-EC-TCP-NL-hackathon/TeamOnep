//
// Created by dlwog on 25. 10. 30..
//

#ifndef MYPROJECT_FLUID_HPP
#define MYPROJECT_FLUID_HPP


#include <vector>
#include <glm/glm.hpp>

struct Particle {
  glm::vec3 position;
  glm::vec3 velocity;
  glm::vec3 force;
  float mass;
  float density;
};

struct FluidSystem {
  std::vector<Particle> particles;
  float h = 1.0f;      // Smoothing length
  float k = 1.0f;      // Gas constant
  float viscosity = 0.1f;
  glm::vec3 gravity = glm::vec3(0,-9.8f,0);

  void computeDensity() {
    for(auto& pi : particles) {
      pi.density = 0.0f;
      for(auto& pj : particles) {
        float r = glm::length(pi.position - pj.position);
        if(r < h) {
          float q = (h*h - r*r);
          pi.density += pj.mass * q*q*q;
        }
      }
    }
  }

  void computeForces() {
    for(auto& pi : particles) {
      glm::vec3 fPressure(0.0f);
      glm::vec3 fViscosity(0.0f);
      for(auto& pj : particles) {
        if(&pi == &pj) continue;
        glm::vec3 rij = pi.position - pj.position;
        float r = glm::length(rij);
        if(r < h) {
          // pressure (simplified)
          float p = k * (pi.density + pj.density - 2.0f);
          fPressure += -p * glm::normalize(rij);

          // viscosity (simplified)
          fViscosity += viscosity * (pj.velocity - pi.velocity);
        }
      }
      pi.force = fPressure + fViscosity + pi.mass * gravity;
    }
  }

  void integrate(float dt) {
    for(auto& p : particles) {
      p.velocity += dt * (p.force / p.mass);
      p.position += dt * p.velocity;
    }
  }

  void step(float dt) {
    computeDensity();
    computeForces();
    integrate(dt);
  }
};



#endif //MYPROJECT_FLUID_HPP