#ifndef ENGINE_HPP
#define ENGINE_HPP
#include <resource/importer.hpp>
#include <scene_graph/mesh.hpp>
#include <render/renderpass_builder.hpp>
#include <io/event_manager.hpp>
#include <resource/resource_manager.hpp>
#include "GPU/context.hpp"
#include "render/renderer.hpp"

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
  EventManager eventManager_;
  ResourceManager resourceManager;
  Renderer  renderer;
  RenderPassBuilder renderpassBuilder;

};

#endif //engine_hpp
