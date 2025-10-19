#ifndef RENDERER_HPP
#define RENDERER_HPP
#include <../../extern/examples/pipeline.hpp>
#include <../resource/renderpass_pool.hpp>
#include <../resource/descriptor_manager.hpp>

#include <../resource/shader_pool.hpp>
#include <../scene_graph/mesh.hpp>
#include <../resource/sampler_builder.hpp>
#include <../resource/texture.hpp>
#include <../resource_pool.hpp>


#include "../../../extern/examples/renderer_resource.hpp"
#include "../GPU/context.hpp"
#include "unique.hpp"
#include "ui.hpp"

struct RenderInitInfo{
  VkDevice device_h;
  ResourcePool *resourceManager;
  VkExtent2D extent;
  VkDescriptorSetLayout *pDescriptorSetLayouts;
  VkRenderPass renderPass;
  uint32_t descriptorSetLayoutCount;
};

enum class ViewMode{
  SINGLE,
  MULTI,
  FPS,
  TPS,
  VR
};

class RenderingSystem{
  friend class Engine;
  friend class EventProcessor;

public:
  RenderingSystem(RenderInitInfo info);
  ~RenderingSystem() {}
  void uploadRenderPass();
  void pushConstant(VkCommandBuffer cmdBuffer);
  void setUp(VkCommandBuffer cmd);

  void draw(VkCommandBuffer cmd, uint32_t currentFrame);
  void setCamera(Camera *cameraP)
  {
    camera = cameraP;
  }

private:
  std::vector<gpu::SwapchainHandle> swapchainHandle_;
  std::vector<gpu::NodeId> drawHandle_;
  std::vector<gpu::NodeId> depthAttachmentHandle_;
  VkPhysicalDevice physical_device_h;
  VkDevice device_h;
  VkQueue graphics_q;
  VkQueue present_q;
  VkSurfaceKHR surface_h;
  VkFormat format;
  gpu::Pipeline backgroundPipeline_;
  gpu::Pipeline pipeline_h;
  VkPipelineLayout pipelineLayout_h;
  VkDescriptorSetLayout *pDescriptorSetLayouts;
  uint32_t descriptorLayoutCount;
  uint32_t present_family;
  uint32_t graphics_family;

  std::vector<BatchContext> batches_;
  ViewMode viewMode = ViewMode::SINGLE;
  PFN_vkCmdSetPolygonModeEXT vkCmdSetPolygonModeEXT;
  VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL;
  VkBool32 depthTest        = VK_TRUE;
  VkBool32 drawBackground   = VK_TRUE;

  std::string fragPath     = "C:/Users/dlwog/OneDrive/Desktop/VkMain-out/source/shader/uv.frag";
  std::string vertPath     = "C:/Users/dlwog/OneDrive/Desktop/VkMain-out/source/shader/vertex.vert";
  std::string fragBackPath = "C:/Users/dlwog/OneDrive/Desktop/VkMain-out/source/shader/sculptor_background.frag";
  std::string VertBackPath = "C:/Users/dlwog/OneDrive/Desktop/VkMain-out/source/shader/sculptor_background.vert";
  VkViewport viewport{};
  VkRect2D scissor{};

  Camera *camera;
  VulkanTexture *texture;
  ResourcePool &pResourceManager;
  VkDeviceSize offsets = 0;
};
#endif