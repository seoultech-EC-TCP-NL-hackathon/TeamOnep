#ifndef RENDERER_HPP
#define RENDERER_HPP
#include "frame_viewer.hpp"
#include "render_pipeline.hpp"
#include "../resource/resource_manager.hpp"
#include "../../../extern/examples/renderer_resource.hpp"
#include "../GPU/context.hpp"
#include "render_target_film.hpp"
#include "ui/ui.hpp"

class RenderPassBuilder
{
  friend class ResourceManager;
  friend class UI;
  friend class EventManager;
  friend class Engine;

  public:
  RenderPassBuilder();
  ~RenderPassBuilder() = default;
  void init();
  void uploadDepthOnlyPass();
  void uploadGBufferWritePass();
  void uploadShadowPass();
  void uploadLightningPass();
  void uploadBloomingExtractPass();
  void uploadBloomingBlurPass();
  void uploadTonemapPass();
  void uploadGammaCorrectionPass();
  void offscreenRenderPass();
  void uploadUiDraw();
  void draw(VkCommandBuffer cmd, uint32_t currentFrame);

  std::vector<RenderTarget> nodes;
  private:
  void buildPass();
  void updateFrameConstant();
  void pushModelConstant(VkCommandBuffer command, ModelConstant* modelConstant);
  void pushFrameConstant(VkCommandBuffer cmdBuffer);
  Pipeline pipeline;
  flm::RenderTragetFilm offscreenFilm_;
  std::vector<Model*> drawHandle_;
  std::vector<std::unique_ptr<gpu::RenderPass>> depthOnlyPass;
  std::vector<std::unique_ptr<gpu::RenderPass>> gBufferPass;
  std::vector<std::unique_ptr<gpu::RenderPass>> uiDrawPass;
  std::vector<std::unique_ptr<gpu::RenderPass>> shadowPass;
  std::vector<std::unique_ptr<gpu::RenderPass>> lightningPass;

  std::vector<std::unique_ptr<gpu::RenderPass>> bloomingExtractPass;
  std::vector<std::unique_ptr<gpu::RenderPass>> bloomingBlurPass;
  std::vector<std::unique_ptr<gpu::RenderPass>> tonemappingPass;
  std::vector<std::unique_ptr<gpu::RenderPass>> gammaPass;

  std::vector<std::unique_ptr<gpu::RenderPass>> swapchainRenderPass;
};
#endif
