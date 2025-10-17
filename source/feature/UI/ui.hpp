
#ifndef UIRENDERER_HPP
#define UIRENDERER_HPP
#include <common.hpp>
#include <../resource/renderpass_pool.hpp>
#include <../../extern/examples/pipeline.hpp>
#include "../sculptor/sculptor_act.hpp"
#include "../../core/io/log_sink.hpp"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include "../../core/resource_pool.hpp"
enum class CallType{
  Mesh,
  Texture,
  UPLOAD,
  UPDATE,
  FREE,
};

struct UICall{
  std::string path;
  uint32_t bindingSlot = -1;
  CallType type;
};

struct UITexture{
  VkDescriptorSet descriptorSet;
  bindingIndex index;
};

class UI{
  friend class Engine;

public:
  UI();
  void uploadUIPass();
  void rec(VkCommandBuffer command);
  void render(VkCommandBuffer command);
  void draw(VkCommandBuffer command);
  void drawcall(VkCommandBuffer command);
  void drawTransition(VkCommandBuffer command);
  void drawStateWindow(ImVec2 size);
  void drawToolBoxRight(ImVec2 size);
  void drawToolBoxLeft(ImVec2 size);
  void drawToolBoxUnder(ImVec2 size);
  void drawToolBoxUnderTexture(ImVec2 size);
  void drawVertexState(ImVec2 size);
  void drawIndexState(ImVec2 size);
  void drawTextureState(ImVec2 size);
  void drawCameraState(ImVec2 size);
  void drawMaterialState(ImVec2 size);
  void drawMouseState(ImVec2 size);
  void drawFramebufferState();
  void drawFrame(ImVec2 size);
  void inputText();

  void drawLightUI(ImVec2 size);
  void drawCameraUI();
  void drawMaterialUI();
  void drawShaderUI();
  void setResourceManager(ResourcePool *resourceManager);

private:
  SculptorMode *sculpting = nullptr;
  EditorMode *editor      = nullptr;
  FPS *fps                = nullptr;


  void colorPickerColor();
  void createPool();
  void setupStyle();
  void uploadImageToUI();
  bool smallUi_ = true; // 작은 창 유지 토글
  float color[4];
  float pos[3];
  bindingIndex selectedTextureBinding = 0;
  std::vector<std::string> uploadedMesh_;
  std::vector<UITexture> uiTextures_;
  std::vector<UICall> callStack_;
  std::shared_ptr<sys::LogSink> sink_;
  VkDescriptorSet backgroundDescriptor_;
  VulkanTexture *backgroundTexture_;
  ResourcePool *pResourceManager_;
};

#endif