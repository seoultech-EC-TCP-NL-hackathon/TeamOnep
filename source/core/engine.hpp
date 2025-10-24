#ifndef ENGINE_HPP
#define ENGINE_HPP
#include  <common.hpp>
#include <resource/importer.hpp>
#include <scene_graph/mesh.hpp>
#include <render/renderer.hpp>
#include <../feature/UI/ui.hpp>
#include <io/event_manager.hpp>
#include <render/resource_manager.hpp>
#include "GPU/context.hpp"

class Engine
{
  public:
  Engine();
  ~Engine();
  void init();
  void run();
  void drawUI();

  private:
  UI ui;
  EventManager EventManager_;
  std::shared_ptr<sys::LogSink> UIsink;
  VkPipelineLayout pipeline_layout_h;
  gpu::VkMemoryAllocator* pAllocator;
  std::unique_ptr<ResourceManager> resourceManager_;
  std::unique_ptr<IRenderer> Renderer;
  VkExtent2D extent = {2400, 1200};
  const char* title = "vulkan";
};

#endif //engine_hpp
