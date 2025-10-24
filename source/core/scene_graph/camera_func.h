#include "spdlog/spdlog.h"
#include"camera_state.hpp"
#include "io.hpp"
#pragma once


namespace mns
{
  struct WSAD;
  struct CursorState;
  extern mns::WSAD WSAD__;
  extern mns::CursorState cursor__;
}

namespace fn_cam
{
  void wsadMove(UserCamera* cam);
  void updateView(UserCamera* cam);
  void rotateCam(UserCamera* cam);
  void rotateModel(UserCamera* cam);
  void rotateCam(UserCamera* cam);
  void resetState(UserCamera* cam);
  void updateWheel(UserCamera* cam);


  inline void debug(UserCamera* camera)
  {
    spdlog::info("dir : {} , {} , {}",
                 camera->dir_.x,
                 camera->dir_.y,
                 camera->dir_.z);
    spdlog::info("pos :  {} , {} , {}",
                 camera->camTransform_.position.x
                 , camera->camTransform_.position.y
                 , camera->camTransform_.position.z);
    spdlog::info("fov :  {} , {} ",
                 camera->cache .lastWheelOffsetX
                 , camera->projection_.fov_);

  }
}
