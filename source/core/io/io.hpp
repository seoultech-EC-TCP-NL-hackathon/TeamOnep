#ifndef INTERACTION_HPP
#define INTERACTION_HPP
#include "GLFW/glfw3.h"


namespace mns{
  class IoSystem;
  extern IoSystem io__;
  class IoSystem{
  public:
    IoSystem();
    void init();
    void onKeyCallback(int key, int scancode, int action, int mods);
    void getKey();
    void getMouseEvent();
    static void keyCallbackWrapper(GLFWwindow *window, int key, int scancode, int action, int mods);
    static void mouseButtonCallbackWrapper(GLFWwindow *window, int button, int action, int mods);
    static void cursorPosCallbackWrapper(GLFWwindow *window, double xpos, double ypos);
    static void framebufferSizeCallback(GLFWwindow *window, int w, int h);
    static void scrollCallback(GLFWwindow *window, double xoffset, double yoffset);
    void cursorPosCallback(GLFWwindow *window, double xpos, double ypos);
    void getViewIndex(double w, double h);
    ImGuiKey glfwToImgui(int key);
    bool dirty_ = false;
    struct{
      bool keySpace     = false;
      bool keyCtrl      = false;
      bool keyAlt       = false;
      bool key0         = false;
      bool key1         = false;
      bool key2         = false;
      bool key3         = false;
      bool key4         = false;
      bool key5         = false;
      bool key6         = false;
      bool key7         = false;
      bool key8         = false;
      bool key9         = false;
      bool keyA         = false;
      bool keyB         = false;
      bool keyC         = false;
      bool keyD         = false;
      bool keyE         = false;
      bool keyF         = false;
      bool keyG         = false;
      bool keyH         = false;
      bool keyI         = false;
      bool keyJ         = false;
      bool keyK         = false;
      bool keyL         = false;
      bool keyM         = false;
      bool keyN         = false;
      bool keyO         = false;
      bool keyP         = false;
      bool keyQ         = false;
      bool keyR         = false;
      bool keyS         = false;
      bool keyT         = false;
      bool keyU         = false;
      bool keyV         = false;
      bool keyW         = false;
      bool keyX         = false;
      bool keyY         = false;
      bool keyZ         = false;
      bool keyEqual     = false;
      bool keySemicolon = false;
    } keyState__;

    struct{
      double xpos          = 0.0;
      double ypos          = 0.0;
      double scrollXOffset = 0.0;
      double scrollYOffset = 0.0;

      bool leftButtonDown   = false;
      bool rightButtonDown  = false;
      bool middleButtonDown = false;
    } mouseState__;

    struct{
      bool resized = false;
      int width    = 0;
      int height   = 0;
    } windowState__;
  };
}
#endif //INTERACTION_HPP