//
// Created by ljh on 25. 9. 21..
//

#ifndef MYPROJECT_CAMERA_FPS_HPP
#define MYPROJECT_CAMERA_FPS_HPP
#include <../../core/scene_graph/camera.hpp>
#include <../../core/scene_graph/camera_cfg.hpp>
struct FPSCamera: Camera{
  FPSCamera(CamCI camCi): Camera(camCi){}
  void moveForward()
  {
    glm::vec3 dir = dir_;
    dir.y= 0;
    pos_+= dir*delta;
  }
  void moveRight()
  {
    glm::vec3 dir = right_;
    dir.y= 0;
    pos_+= dir*delta;
  }
  void moveLeft()
  {
    glm::vec3 dir = -right_;
    dir.y= 0;
    pos_+= dir*delta;
  }
  void moveBackward()
  {
    glm::vec3 dir = -dir_;
    dir.y= 0;
    pos_+= dir*delta;
  }
};
#endif //MYPROJECT_CAMERA_FPS_HPP