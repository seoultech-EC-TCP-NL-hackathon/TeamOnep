//
// Created by dlwog on 25. 10. 30..
//

#ifndef MYPROJECT_ASYN_FIELD_HPP
#define MYPROJECT_ASYN_FIELD_HPP


#include <vector>
#include <glm/glm.hpp>
#include <random>

struct AsyncNode {
  glm::vec3 position;
  glm::vec3 velocity;
  float lastUpdate;
  float updateInterval;

  void update(float currentTime) {
    if(currentTime - lastUpdate >= updateInterval) {
      // 독립적 상태 갱신
      position += velocity * updateInterval;

      // 랜덤 변동
      static std::mt19937 gen(0);
      static std::uniform_real_distribution<float> dis(-0.1f,0.1f);
      velocity += glm::vec3(dis(gen), dis(gen), dis(gen));

      lastUpdate = currentTime;
    }
  }
};

struct AsyncSystem {
  std::vector<AsyncNode> nodes;

  void step(float time) {
    for(auto& node : nodes) {
      node.update(time);
    }
  }
};

#endif //MYPROJECT_ASYN_FIELD_HPP