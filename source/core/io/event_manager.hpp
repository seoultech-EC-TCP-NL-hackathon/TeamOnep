#ifndef Enteraction__
#define Enteraction__
#include "../render_graph/renderer.hpp"
#include "../sculptor/sculptor_act.hpp"
#include "../../feature/User/user.hpp"
#include "imgui.h"


class EventProcessor
{
  friend class Engine;
  public:
  EventProcessor() = default;
  ~EventProcessor() = default;
  ImGuiKey glfwToImgui(int key);
  void processKeyEvent();

  void getKey();
  void getMouseEvent();
  void wheelUpdate();
  void setRenderer(RenderingSystem* renderer);
  void selectActor();

  private:
  static void keyCallbackWrapper(GLFWwindow* window, int key, int scancode, int action, int mods);
  static void mouseButtonCallbackWrapper(GLFWwindow* window, int button, int action, int mods);
  static void cursorPosCallbackWrapper(GLFWwindow* window, double xpos, double ypos);
  static void framebufferSizeCallback(GLFWwindow* window, int w, int h);
  static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
  void onMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
  void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
  void getViewIndex(double w, double h);

  private:
  bool singleModel = true;
  UserMode currentActor_ = UserMode::Sculptor;
  std::unique_ptr<User> actor_;
  ResourcePool* resourcesManager_;
  RenderingSystem* renderer_;
  GLFWwindow* window_;
  Camera* mainCam;
  bool altPressed = false;
  bool leftButton = false;
  bool middleButton = false;
  bool rightButton = false;

  float yaw = -90.0f;
  float pitch = 0.0f;
  float distance = 5.0f;

  bool moved = false;
  double wheelDelta_ = 0;
  bool muliiViews = false;
  bool VR = false;
  bool resized = false;
  bool leftMousePressed = false;
  float sensitivity = 0.05f;
  double lastX = 0.0;
  double lastY = 0.0;
  double lastActionTime;
  bool mouseMoveState = true;
  const double actionCooldown = 0.2; // 200ms
  VkExtent2D currentExtent;
};
#endif //INTERACTION_HPP
