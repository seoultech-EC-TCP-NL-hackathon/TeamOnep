//
// Created by dlwog on 25. 10. 20..
//
#include "transform.h"
#include "camera_func.h"
#include "io.hpp"
#include "User/user.hpp"

namespace fn_cam
{
  void wsadMove(UserCamera* cam)
  {
    if (*mns::WSAD__.W)cam->camTransform_.position += cam->dir_ * cam->delta;
    if (*mns::WSAD__.S)cam->camTransform_.position -= cam->dir_ * cam->delta;
    if (*mns::WSAD__.D)cam->camTransform_.position += cam->right_ * cam->delta;
    if (*mns::WSAD__.A)cam->camTransform_.position -= cam->right_ * cam->delta;
  }

  void updateView(UserCamera* cam)
  {
    cam->ubo.camPos = cam->camTransform_.position;
    cam->ubo.view = glm::lookAt(cam->camTransform_.position,
                                cam->camTransform_.position + cam->dir_,
                                cam->up_);
    cam->ubo.proj = glm::perspective(cam->projection_.fov_,
                                     cam->projection_.aspect,
                                     cam->projection_.nearPlane,
                                     cam->projection_.farPlane
                                    );
    cam->ubo.proj[1][1] *= -1.0f;
  }

  void rotateCam(UserCamera* cam)
  {
    float deltaX = (*mns::cursor__.xPos) - cam->cache.lastX;
    float deltaY = (*mns::cursor__.yPos) - cam->cache.lastY;
    cam->camTransform_.rotate(deltaY,
                              -deltaX);
    cam->dir_ = glm::normalize(cam->camTransform_.rotation * glm::vec3(0, 0, -1));
  }

  void resetState(UserCamera* cam)
  {
    if (mns::io__.keyState__.keyF)
    {
      cam->camTransform_.position = glm::vec3(0, 0, 0);
      cam->dir_ = glm::vec3(0, 0, -1);
      cam->up_ = glm::vec3(0, 1, 0);
      cam->right_ = glm::vec3(1, 0, 0);
      cam->camTransform_.rotation = glm::quat();
    }
  }

  void updateWheel(UserCamera* cam)
  {
    //float deltaX = (*mns::cursor__.wheelXoffset) - cam->cache.lastWheelOffsetX;
    float deltaY = (*mns::cursor__.wheelYoffset) - cam->cache.lastWheelOffsetY;
    cam->projection_.fov_ += deltaY * 3;
  }

  //Ray Camera::generateRay(double posX, double posY)
  //{
  //  double xNdc = (2 * posX / currentExtent.width) - 1;
  //  double yNdc = 1 - (2 * posY / currentExtent.height);
  //
  //  float tanFov = tan(fov_ * 0.5f);
  //  float xView = xNdc * aspect * tanFov;
  //  float yView = yNdc * tanFov;
  //  glm::vec3 rayDir = glm::vec3(-xView, yView, -1.0f);
  //  rayDir = glm::normalize(rayDir);
  //  Ray ray{pos_, rayDir};
  //  return ray;
  //}

  //void Camera::onVR(bool left)
  //{
  //  float top = nearPlane * tan(VRFov / 2);
  //  glm::vec3 leftEye = pos_;
  //  float bottom = -top;
  //  float right = top * aspect;
  //  float leftPoint = -right;
  //  // glm::mat4 frustom = glm::frustum(leftPoint, right, bottom, top, nearPlane, farPlane);
  //  //ubo.proj = frustom;
  //
  //  glm::vec3 eyePos = pos_;
  //  if (left) eyePos -= ipdHalf * right_;
  //  if (!left)eyePos += ipdHalf * right_;
  //  glm::mat4 View = glm::lookAt(eyePos, eyePos + dir_, up_);
  //  //  ubo.view = View;
  //}
}
