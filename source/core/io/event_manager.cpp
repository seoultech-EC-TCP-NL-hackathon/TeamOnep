#include <event_manager.hpp>
#include "io.hpp"
#include "../sculptor/sculptor_act.hpp"

//EventProcessor_->getKey();
//EventProcessor_->wheelUpdate();
//EventProcessor_->getMouseEvent();
//uiRenderer->uploadImageToUI();
//EventProcessor_->moved  = false;


void EventProcessor::processKeyEvent()
{
  if (mns::io__.dirty_)
  {
    auto& state = mns::io__.keyState__;
    // if (state.keySpace) renderer_->polygonMode = VK_POLYGON_MODE_FILL;
    // if (state.keyCtrl) renderer_->polygonMode = VK_POLYGON_MODE_LINE;
    // if (state.keyAlt) renderer_->depthTest = !renderer_->depthTest;
    //if (state.key0) renderer_->viewMode = ViewMode::VR;
    //if (state.key1) renderer_->viewMode = ViewMode::SINGLE;
    if (state.keyQ) glfwSetWindowShouldClose(window_, GLFW_TRUE);
  }
}
  void EventProcessor::getMouseEvent()
  {
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse)
    {
      return;
    }
    actor_->getMouseEvent();
  }

  void EventProcessor::wheelUpdate()
  {
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse)
    {
      return;
    }

    actor_->getWheelUpdate(wheelDelta_);
    wheelDelta_ = 0.0f;
  }

  void EventProcessor::setRenderer(RenderingSystem* renderer)
  {
    renderer_ = renderer;
  }

  void EventProcessor::selectActor()
  {
    switch (currentActor_)
    {
      case(UserMode::Sculptor):
        {
          actor_ = std::make_unique<SculptorMode>(window_,
                                                  mainCam,
                                                  &resourcesManager_->selectedModel,
                                                  currentExtent);
          break;
        }
      case(UserMode::FPS):
        {
          actor_ = std::make_unique<FPS>(window_, mainCam);
          break ;
        }
      default:
        {
          break;
        }
    }
  }


// void EventProcessor::getViewIndex(double w, double h)
// {
//   ImGuiIO& io = ImGui::GetIO();
//   if (io.WantCaptureMouse)
//   {
//     return;
//   }
//   bool right = w >= (currentExtent.width / 2.0f);
//   bool top = h >= (currentExtent.height / 2.0f);
//   int index = 0;
//   if (!right && !top) index = 0;
//   else if (right && !top) index = 1;
//   else if (!right && top) index = 2;
//   else if (right && top) index = 3;
//   //mainCam->setMainCam(index);
// }

//  muliiViews = !muliiViews;
//  if (!muliiViews)
//  {
//    renderer_->viewMode = ViewMode::SINGLE;
//    double mouseX, mouseY;
//    glfwGetCursorPos(window_, &mouseX, &mouseY);
//    getViewIndex(mouseX, mouseY);
//  }
//  else
//  {
//    renderer_->viewMode = ViewMode::MULTI;
//  }
//}