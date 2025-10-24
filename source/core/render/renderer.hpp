#ifndef RENDERER_HPP
#define RENDERER_HPP
#include <../scene_graph/mesh.hpp>
#include <../resource/sampler_builder.hpp>
#include <../GPU/texture.hpp>
#include "resource_manager.hpp"
#include "../../../extern/examples/renderer_resource.hpp"
#include "../GPU/context.hpp"
#include "unique.hpp"
#include "ui.hpp"

struct RenderInitInfo
{
  ResourceManager* resourceManager;
};

enum class ViewMode
{
  SINGLE,
  MULTI,
  FPS,
  TPS,
  VR
};

class IRenderer
{
  friend class Engine;
  friend class EventManager;
  public:
  IRenderer(RenderInitInfo info);
  ~IRenderer() = default;

  void uploadDepthPass();
  void uploadQuadDraw();
  void uploadUiDraw();

  void pushConstant(VkCommandBuffer cmdBuffer);
  void draw(VkCommandBuffer cmd, uint32_t currentFrame);

  private:
  std::vector<gpu::SwapchainHandle> swapchainHandle_;
  std::vector<gpu::VkMeshBuffer*> drawHandle_;
  std::vector<gpu::VkFrameAttachment*> depthAttachmentHandle_;
  gpu::Pipeline backgroundPipeline_;
  gpu::Pipeline pipeline_h;
  gpu::PipelineLayout pipelineLayout_h;
  VkDescriptorSetLayout* pDescriptorSetLayouts;
  uint32_t descriptorLayoutCount;
  std::vector<BatchContext> batches_;
  ViewMode viewMode = ViewMode::SINGLE;
  PFN_vkCmdSetPolygonModeEXT vkCmdSetPolygonModeEXT;
  VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL;
  VkBool32 depthTest = VK_TRUE;
  VkBool32 drawBackground = VK_TRUE;

  std::string fragPath = "C:/Users/dlwog/OneDrive/Desktop/VkMain-out/source/shader/depth.frag";
  std::string vertPath = "C:/Users/dlwog/OneDrive/Desktop/VkMain-out/source/shader/vertex.vert";
  std::string fragBackPath = "C:/Users/dlwog/OneDrive/Desktop/VkMain-out/source/shader/def_depth.frag";
  std::string VertBackPath = "C:/Users/dlwog/OneDrive/Desktop/VkMain-out/source/shader/quad.vert";
  VkViewport viewport{};
  VkRect2D scissor{};

  struct FrameAttachment
  {
    int gBufferPositon = 0;
    int gBufferNormal = 0;
    int gBufferAlbedo = 0;
    int gBufferRoughness = 0;

    int DepthBuffer = 0;
    int ShaderBuffer = 0;
    int lightningBuffer = 0;
    int postProcessBuffer = 0;

    glm::mat4 modelMatrix;
    glm::vec4 vec4;
    glm::vec4 vec3;
  } renderAttachment;

  VulkanTexture* texture;
  ResourceManager& pResourceManager;
  VkDeviceSize offsets = 0;
};
#endif
