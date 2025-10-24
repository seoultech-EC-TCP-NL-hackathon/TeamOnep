#define   STB_IMAGE_IMPLEMENTATION
#include <engine.hpp>
#include "util/unique.hpp"
#include "../feature/UI/ui.hpp"
#include "spdlog/spdlog.h"
#include "GPU/context.hpp"
#include "io/log_sink.hpp"
#include "io/io.hpp"
#include "ui.hpp"
#include "../scene_graph/camera_state.hpp"
///todo:
/// RI SETTING :
/// 1. PIPELINE
/// 2. CONTEXT
/// 3. COMMAND BUFFER WRAPPER
///
///todo:
/// Resource UPDATE Setting
/// 1. Mesh Upload on pannel
/// 2. TEXTURE UPLOAD AND Deffered Rendering
/// 3. DLSS
/// 4. RT setting
/// 5. compute pass Ascync
///
///
///
///
///
Engine::Engine() = default;

void Engine::init()
{
  gpu::ctx__->loadContext();
  mns::io__.init();
  pAllocator = gpu::ctx__->pMemoryAllocator.get();
  spdlog::info("create Image Manager");
  ResourceManagerCreateInfo resourceCi;
  resourceCi.device = gpu::ctx__->deviceh__;
  resourceCi.allocator = pAllocator;
  resourceManager_ = std::make_unique<ResourceManager>(resourceCi);
  spdlog::info("init renderer");
  RenderInitInfo renderinfo{};
  renderinfo.resourceManager = resourceManager_.get();
  Renderer = std::make_unique<IRenderer>(renderinfo);
  pipeline_layout_h = Renderer->pipelineLayout_h;
  ui.setupStyle();
  ui.setResourceManager(resourceManager_.get());
  Mesh mesh = resourceManager_->uploadMesh("C:/Users/dlwog/OneDrive/Desktop/VkMain-out/assets/models/hand.fbx");
  mns::uptr<gpu::VkMeshBuffer> meshNode = mns::mUptr<gpu::VkMeshBuffer>();
  meshNode->vertex = mesh.vertices;
  meshNode->indices = mesh.indices;
  meshNode->hostUpdate__ = false;
  meshNode->dirty__ = true;
  auto ptr= gpu::ctx__->graphBuilder.registerMeshBuffer(meshNode);
  Renderer->drawHandle_.push_back(ptr);
}


void Engine::run()
{
  UIsink = std::make_shared<sys::LogSink>();
  //spdlog::set_default_logger(std::make_shared<spdlog::logger>("default", UIsink));
  //spdlog::set_level(spdlog::level::trace);
  init();
  ui.sink_ = UIsink;
  //uIRenderer->uploadImageToUI();
  gpu::Scheduler scheduler(gpu::ctx__);
  while (!glfwWindowShouldClose(gpu::ctx__->windowh__))
  {
    glfwPollEvents();
    (scheduler.nextFrame());
    ui.update();
    resourceManager_->updateMaincamState((gpu::ctx__->renderingContext.currentFrame__ + 1) %
                                          gpu::ctx__->renderingContext.maxInflight__);
    EventManager_.moveProcessEvent();
    Renderer->uploadDepthPass();
    Renderer->uploadQuadDraw();
    Renderer->uploadUiDraw();
    scheduler.run();
  }
}


Engine::~Engine()
{
};
