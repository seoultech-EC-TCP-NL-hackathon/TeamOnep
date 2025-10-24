#include "sculptor_act.hpp"
#include "glm/gtx/quaternion.hpp"
#include "../User/user.hpp"
#include "sculptor.hpp"
constexpr float MODEL_VIEW_TRANSLATE  = 0.01f;
constexpr float MODEL_DEPTH_TRANSLATE = 2.0f;
constexpr float MODEL_SCALING         = 0.01;
constexpr float EPSILON               = 1e-5f;

/// sculptor don't use camera
/// fixed cam mode

//void UI::drawMouseState(ImVec2 size)
//{
//  if (sculpting != nullptr)
//  {
//    ImDrawList* draw_list = ImGui::GetForegroundDrawList();
//    ImVec2 mouse_pos = ImGui::GetIO().MousePos;
//    float radius = sculpting->sculptor->brush->radius;
//    ImU32 mouseColor = IM_COL32(255, 0, 0, 255);
//    float thickness = 2.0f;
//    draw_list->AddCircle(mouse_pos, radius, mouseColor, 32, thickness);
//    if (sculpting->symmetry_)
//    {
//      // show symmetry
//      ImVec2 point = ImGui::GetIO().DisplaySize;
//      point.x -= mouse_pos.x;
//      point.y = mouse_pos.y;
//      draw_list->AddCircle(point, radius, mouseColor, 32, thickness);
//    }
//  }
//}
SculptorMode::SculptorMode(GLFWwindow *window,
                           Camera *camera,
                           Model *model,
                           VkExtent2D extent) :
  User(window, camera),
  extent(extent)
{
  sculptingModel = model;
  sculptor       = std::make_unique<Sculptor>(model, extent);
  camera_        = dynamic_cast<EditorCam *>(camera_);
  sculptor->createZBuffer(camera_->ubo.proj, camera_->ubo.view);
  shoudAct = true;
}

void SculptorMode::keyEvent(int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_O and GLFW_PRESS)
  {
    sculptor->dynamicMesh_.subdivideMesh();
    shoudAct = true;
    return;
  }
  if (key == GLFW_KEY_F and GLFW_KEY_LEFT_CONTROL and GLFW_PRESS)
  {
    sculptingModel->transform.position   = glm::vec3(0, 0, 0);
    sculptingModel->transform.rotation   = glm::quat{};
    sculptingModel->constant.modelMatrix = sculptingModel->transform.getMatrix();
  }
  if (key == GLFW_KEY_S and action == GLFW_PRESS)
  {
    symmetry_ = !symmetry_;
    return;
  }
  if (key == GLFW_KEY_LEFT_ALT and action == GLFW_PRESS)
  {
    sculptor->brush->strength *= -1;
    return;
  }
  if (key == GLFW_KEY_G and action == GLFW_PRESS)
  {
    sculptor->dynamicMesh_.smoothVertices();
    shoudAct = true;
  }
}

void SculptorMode::getWheelUpdate(float deltaS)
{
  sculptingModel->transform.position -= deltaS * MODEL_DEPTH_TRANSLATE * glm::vec3(0, 0, -1);
  sculptingModel->constant.modelMatrix = sculptingModel->transform.getMatrix();
}

void SculptorMode::act(VkCommandBuffer command)
{
  sculptor->dynamicMesh_.dynMeshUpdate(command);
  shoudAct = false;
}

void SculptorMode::cursorPosCallBack(float deltaX, float deltaY)
{
  deltaX_ = deltaX;
  deltaY_ = deltaY;
}

void SculptorMode::getKey() {}

void SculptorMode::getMouseEvent()
{
  if (sculptor->dynamicMesh_.dirty_)
  {
    shoudAct = true;
    return;
  }
  if (glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
  {
    if ((glfwGetKey(window_, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS))
    {
      glm::quat modelRotate = sculptingModel->transform.rotation;
      glm::quat yawRotation = glm::angleAxis(glm::radians(deltaX_), glm::vec3(0, 1, 0));

      modelRotate = yawRotation * modelRotate;
      modelRotate = glm::normalize(modelRotate);

      sculptingModel->transform.rotation   = modelRotate;
      sculptingModel->constant.modelMatrix = sculptingModel->transform.getMatrix();
      deltaX_                              = 0;
      deltaY_                              = 0;
      // TODO :  RECOMPUTE  ZBUFFER
      return;
    }
    if ((glfwGetKey(window_, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS))
    {
      // TODO : PITCH ROTATION REVERSE
      //glm::quat modelRotate = sculptingModel->transform.rotation;
      //glm::quat pitchRotate = glm::angleAxis(glm::radians(deltaY_), glm::vec3(1, 0, 0));
      //modelRotate           = pitchRotate * modelRotate;
      //modelRotate           = glm::normalize(modelRotate);
      //
      //sculptingModel->transform.rotation   = modelRotate;
      //sculptingModel->constant.modelMatrix = sculptingModel->transform.getMatrix();
      //deltaX_                              = 0;
      //deltaY_                              = 0;
      //return;
    }
    double x, y;
    glfwGetCursorPos(window_, &x, &y);
    x += sculptingModel->transform.position.x;
    y += sculptingModel->transform.position.y;

    int xInt = std::clamp(static_cast<int>(std::round(x)),
                          0,
                          static_cast<int>(extent.width - 1));
    int yInt = std::clamp(static_cast<int>(std::round(y)),
                          0,
                          static_cast<int>(extent.height - 1));
    spdlog::debug("x: {}, y: {}", x, y);
    float minDepth = 1;
    for (int dy = -sculptor->brush->radius; dy <= sculptor->brush->radius; ++dy)
    {
      for (int dx = -sculptor->brush->radius; dx <= sculptor->brush->radius; ++dx)
      {
        int nx   = std::clamp(xInt + dx, 0, static_cast<int>(extent.width - 1));
        int ny   = std::clamp(yInt + dy, 0, static_cast<int>(extent.height - 1));
        minDepth = std::min(minDepth, sculptor->zBuffer.depth[ny][nx]);
      }
    }
    if ((minDepth) > 0.99)
    {
      spdlog::trace("depth :{}", (sculptor->zBuffer.depth[yInt][xInt]));
      spdlog::trace("not hitted");
      return;
    }
    float ndcX = ((x + 0.5f) / extent.width) * 2.0f - 1.0f;
    float ndcY = 1.0f - ((y + 0.5f) / extent.height) * 2.0f;
    float ndcZ = sculptor->zBuffer.depth[yInt][xInt];
    glm::vec4 ndcPos(ndcX, ndcY, ndcZ, 1.0f);
    glm::vec4 viewPos = glm::inverse(camera_->getProjectionMatrix()) * ndcPos;
    viewPos /= viewPos.w;
    glm::vec4 worldPos = glm::inverse(camera_->getViewMatrix()) * viewPos;
    worldPos.w         = 0;
    glm::vec4 modelPos = glm::inverse(sculptingModel->transform.getMatrix()) * worldPos;
    glm::vec3 modelDir = glm::normalize(glm::vec3(modelPos));
    modelDir.y *= -1;
    glm::quat rotation     = sculptingModel->transform.rotation;
    glm::vec3 modelForward = rotation * glm::vec3(0, 0, -1);
    float dotProd          = glm::dot(glm::normalize(modelForward), glm::vec3(0, 0, 1));
    bool flipped           = dotProd < 0.0f;

    if (sculptor->stroke(modelDir))
    {
      if (symmetry_)
      {
        modelDir.x *= -1;
        sculptor->stroke(modelDir);
      }
      //sculptor->dynamicMesh_.SmoothVertices();
      sculptor->depthUpdate(xInt, yInt, ndcZ);
      shoudAct = true;
    }
    deltaX_ = 0;
    deltaY_ = 0;
    return;
  }
  if (glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
  {
    sculptingModel->transform.position +=
    deltaX_ * MODEL_VIEW_TRANSLATE * glm::vec3(1, 0, 0) -
    deltaY_ * MODEL_VIEW_TRANSLATE * glm::vec3(0, 1, 0);
    sculptingModel->constant.modelMatrix = sculptingModel->transform.getMatrix();

    deltaX_ = 0;
    deltaY_ = 0;
    return;
  }

  if (glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
  {
    if ((glfwGetKey(window_, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS))
    {
      sculptor->brush->radius -= deltaY_;
      if (sculptor->brush->radius < 1)
      {
        sculptor->brush->radius = 1;
      }
      deltaX_ = 0;
      deltaY_ = 0;
      return;
    }
    sculptingModel->transform.scale += deltaX_ * MODEL_SCALING;
    sculptingModel->constant.modelMatrix = sculptingModel->transform.getMatrix();
    deltaX_                              = 0;
    deltaY_                              = 0;
    return;
  }
}