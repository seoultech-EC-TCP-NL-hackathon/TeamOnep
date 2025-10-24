#include <event_manager.hpp>
#include "io.hpp"
#include "../sculptor/sculptor_act.hpp"

//EventManager_->getKey();
//EventManager_->wheelUpdate();
//EventManager_->getMouseEvent();
//uIRenderer->uploadImageToUI();
//EventManager_->moved  = false;

void EventManager::moveProcessEvent()
{
  if (mns::io__.dirty_)
  {
    auto& state = mns::io__.keyState__;
    if (state.keySpace) renderer_->polygonMode = VK_POLYGON_MODE_FILL;
    if (state.keyCtrl) renderer_->polygonMode = VK_POLYGON_MODE_LINE;
    if (state.keyAlt) renderer_->depthTest = !renderer_->depthTest;
    if (state.key0) renderer_->viewMode = ViewMode::VR;
    if (state.key1) renderer_->viewMode = ViewMode::SINGLE;
    if (state.keyQ) glfwSetWindowShouldClose(window_, GLFW_TRUE);
    if (state.keyT) ;
  }
}

void EventManager::getMouseEvent()
{
  ImGuiIO& io = ImGui::GetIO();
  if (io.WantCaptureMouse)
  {
    return;
  }
  actor_->getMouseEvent();
}
