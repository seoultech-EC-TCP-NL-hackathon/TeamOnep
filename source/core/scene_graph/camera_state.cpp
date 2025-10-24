#include "camera_state.hpp"
#include "camera_func.h"
#include "io.hpp"

UserCamera::UserCamera()
  : camTransform_(glm::vec3(0.0f, 0.0f, 0.0f)),
    dir_(0.0f, 0.0f, -1.0f),
    up_(0.0f, 1.0f, 0.0f),
    delta(0.001),
    projection_{}
{
  projection_.farPlane = 1000.0f;
  switch (mode)
  {
    default:
    {
      moveProcess = fn_cam::wsadMove;
      updateViewMatrix = fn_cam::updateView;
      rotateProcess = fn_cam::rotateCam;
      zoomProcess = fn_cam::updateWheel;
      resetState = fn_cam::resetState;
    }
  }
}

void UserCamera::update()
{
  moveProcess(this);
  rotateProcess(this);
  zoomProcess(this);
  resetState(this);
  updateViewMatrix(this);
  this->cache.lastX = *mns::cursor__.xPos;
  this->cache.lastY = *mns::cursor__.yPos;
  this->cache.lastWheelOffsetX = *mns::cursor__.wheelXoffset;
  this->cache.lastWheelOffsetY = *mns::cursor__.wheelYoffset;
}
