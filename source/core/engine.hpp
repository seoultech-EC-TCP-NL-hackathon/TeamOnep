#ifndef ENGINE_HPP
#define ENGINE_HPP

#include  <common.hpp>
#include  <importer.hpp>
#include <scene_graph/mesh.hpp>
#include <render/renderer.hpp>
#include <../feature/UI/ui.hpp>
#include <io/event_manager.hpp>
#include  <resource_pool.hpp>
#include "GPU/context.hpp"
#include "swapchain_view.hpp"

class Engine{
public:
  Engine();
  ~Engine();
  void init();
  void run();
  void drawUI();

private:
  UI ui;
  EventProcessor eventProcessor_;
  std::shared_ptr<sys::LogSink> UIsink;
  VkRenderPass renderpass_h;
  VkPipelineLayout pipeline_layout_h;
  gpu::VkMemoryAllocator* pAllocator;
  std::unique_ptr<ResourcePool> resourceManager_;
  std::unique_ptr<RenderingSystem> Renderer;


  VkExtent2D extent  = {2400, 1200};
  const char *title  = "vulkan";
};

#endif //engine_hpp