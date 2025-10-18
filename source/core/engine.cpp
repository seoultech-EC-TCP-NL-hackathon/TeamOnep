#define   STB_IMAGE_IMPLEMENTATION


#include <engine.hpp>
#include "util/unique.hpp"
#include "../feature/UI/ui.hpp"
#include "spdlog/spdlog.h"
#include "GPU/context.hpp"
#include "io/log_sink.hpp"
#include "io/io.hpp"
#include "ui.hpp"
///todo:
/// io sync
/// ui error
/// rendering pass
/// render pass parameter build
/// io sync
/// compute pass and GUI setting, node base tensor graph build

Engine::Engine() = default;
void Engine::init()
{
  gpu::ctx__.loadContext();
  mns::io__.init();
  pAllocator = gpu::ctx__.pMemoryAllocator.get();
  spdlog::info("create Image Manager");

  ResourceManagerCreateInfo resourceCi;
  resourceCi.device = gpu::ctx__.deviceh__;
  resourceCi.allocator = pAllocator;
  resourceManager_ = std::make_unique<ResourcePool>(resourceCi);

  Camera* cam = resourceManager_->getCamera();
  spdlog::info("init renderer");
  RenderInitInfo renderinfo{};
  renderinfo.extent = extent;
  renderinfo.resourceManager = resourceManager_.get();
  renderinfo.pDescriptorSetLayouts = (resourceManager_->descriptorManager->getLayouts()->data());
  renderinfo.descriptorSetLayoutCount = static_cast<uint32_t>(resourceManager_->descriptorManager->getLayouts()->
    size());
  Renderer = std::make_unique<RenderingSystem>(renderinfo);
  pipeline_layout_h = Renderer->pipelineLayout_h;
  ui.setupStyle();
  ui.setResourceManager(resourceManager_.get());
  Renderer->setCamera(cam);

  mns::uptr<gpu::RenderPass> pass = mns::mUptr<gpu::RenderPass>();
  Mesh mesh = resourceManager_->uploadMesh("C:/Users/dlwog/OneDrive/Desktop/VkMain-out/assets/models/sphere.gltf");
  mns::uptr<gpu::VkMeshBuffer> meshNode = mns::mUptr<gpu::VkMeshBuffer>();
  meshNode->vertex = mesh.vertices;
  meshNode->indices = mesh.indices;
  meshNode->hostUpdate__ = false;
  meshNode->dirty__ = true;
  gpu::NodeId handle = gpu::ctx__.pGraphBuilder->buildMeshBuffer(meshNode);
  Renderer->drawHandle_.push_back(handle);
  ImGuiIO& io = ImGui::GetIO();
  io.BackendFlags |= ImGuiBackendFlags_RendererHasTextures;
}


void Engine::run()
{
  UIsink = std::make_shared<sys::LogSink>();
  //spdlog::set_default_logger(std::make_shared<spdlog::logger>("default", UIsink));
  //spdlog::set_level(spdlog::level::trace);
  init();
  ui.sink_ = UIsink;
  //setUp();
  Camera* cam = resourceManager_->getCamera();
  Renderer->setCamera(cam);
  //uiRenderer->uploadImageToUI();
  gpu::Scheduler scheduler(&gpu::ctx__);
  while (!glfwWindowShouldClose(gpu::ctx__.windowh__))
  {
    glfwPollEvents();
    eventProcessor_.processKeyEvent();
    Renderer->uploadRenderPass();
    ui.uploadUIPass();
    scheduler.runGraphicsPipeline();
  }
}

Engine::~Engine()
{
};
