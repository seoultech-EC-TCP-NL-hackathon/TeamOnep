//
// Created by ljh on 25. 9. 19..
//

#ifndef MYPROJECT_SCULPTOR_ACT_HPP
#define MYPROJECT_SCULPTOR_ACT_HPP
#include "../User/user.hpp"
#include "sculptor.hpp"

struct SculptorMode : User{
  SculptorMode(GLFWwindow *window,Camera*camera,  Model *model, VkExtent2D extent);
  void act(VkCommandBuffer command) override;
  virtual void keyEvent(int key, int scancode, int action, int mods) override;
  virtual void getWheelUpdate(float deltaS) override;
  virtual void cursorPosCallBack(float deltaX, float deltaY) override;
  virtual void getKey() override;
  virtual void getMouseEvent() override;
  Model *sculptingModel;
  bool symmetry_ = false;
  VkExtent2D extent;
  float yaw_;
  float pitch_;
};

#endif //MYPROJECT_SCULPTOR_ACT_HPP