#ifndef MYPROJECT_RENDER_GRAPH_NODE_HPP
#define MYPROJECT_RENDER_GRAPH_NODE_HPP

#include <cstdint>
#include <vector>
#include <string>
#include <vulkan/vulkan.h>
#include <functional>
#include <iterator>
#include <optional>
#include <queue>
#include <glm/glm.hpp>
#include <unordered_set>
#include "enum_op.hpp"
#include "../node.hpp"


namespace gpu
{
  using VkRenderPassType = RenderPassType;
  using VkDescriptorFlag = DescriptorFlag;
  using VkResourceUsage = ResourceUsage;
  using VkResourceType = ResourceType;
  using VkMemorySpace = MemorySpace;
  using VkSubMesh = SubMesh;


  constexpr uint32_t VK_VERTEX_BINDING = 0;
  constexpr uint32_t VK_INDEX_BINDING = 0;
  constexpr uint32_t VK_INSTANCE_BINDING = 1;
  constexpr uint32_t VK_GLOBAL_LAYOUT = 0;
  constexpr uint32_t VK_BINDLESS_TEXTURE = 0;
  constexpr uint32_t VK_CAMERA_BINDING = 1;
  constexpr uint32_t VK_GLOBAL_LIGHT = 2;
  constexpr uint32_t VK_LOCAL_LIGHT = 3;
  constexpr uint32_t VK_SHADOW_LAYOUT = 4;
  constexpr uint32_t VK_TEXTURE_LAYOUT = 1;
  constexpr uint32_t VK_ALBEDO_BINDING = 0;
  constexpr uint32_t VK_NORMAL_BINDING = 1;
  constexpr uint32_t VK_ROUGHNESS_BINDING = 2;
  constexpr uint32_t VK_BINDELSS_TEXTURE_ARRAY_COUNT = 1024;
  constexpr uint32_t VK_DYNAMIC_UBO_ARRAY_COUNT = 8;

  using VkPassId = uint32_t;
  using VkNodeId = uint32_t;

  class VkPass;


  enum class VkResourceLifetime
  {
    TRANSFER   = 0,
    PERSISTENT = 1,
    REFERENCE  = 2
  };

  enum class VkComputePassCMD
  {
    ///todo:
    ///compute pass lambda build dispatcher
  };


  class VkNode
  {
    public:
    friend class VkGraphCompiler;
    friend class VkGraphBuilder;
    friend class VkResourceAllocator;
    friend class VkGraph;
    friend class VkDiscardPool;
    VkBool32 masked = false;
    std::string nodeName_;
    uint32_t descriptorSetId;
    uint32_t descriptorBindingId;
    ShaderFlag shaderFlag;
    DescriptorFlag descriptorFlag;
    ResourceType type_;
    ResourceUsage usage_;
    MemorySpace mSpace_;
    bool hostUpdate__ = false;
    bool dirty__ = false;
    private:

    VkMemoryPropertyFlags mFlags_ = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    uint32_t nodeId_ = 0;
    bool allocated__ = false;
    bool binded__ = false;
    uint32_t referenceCount_ = 0;
    uint32_t currentPipeline__ = VK_PIPELINE_STAGE_TRANSFER_BIT;
    uint32_t writePipeline__ = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    VkAccessFlags writeAccessMask__ = VK_ACCESS_MEMORY_READ_BIT;
    VkAccessFlags currentAccessMask__ = VK_ACCESS_NONE;
    VkPass* lastWriter__ = nullptr;
    VkResourceLifetime lifetime = VkResourceLifetime::PERSISTENT;
  };

  class VkBufferNode : public VkNode
  {

    friend class VkGraphCompiler;
    friend class VkGraphBuilder;
    friend class VkResourceAllocator;
    friend class VkGraph;
    friend class VkDiscardPool;

    public:
    void* data_;
    VkDeviceSize size_;
    VkBuffer bufferh_;

    private:
    VkBool32 descriptorAllocated__ = false;
    VkPipelineStageFlagBits writePipelineStage__;
    VkPipelineStageFlagBits currentPipelineStage__;
    VkBufferUsageFlags usage__;
    VkAccessFlagBits currentAccess__ = VK_ACCESS_NONE;
    VkAccessFlagBits writeAccess__ = VK_ACCESS_NONE;
    std::vector<VkDescriptorSet> descriptorSet;
  };

  class VkMeshNode : public VkNode
  {
    void cmdDraw(VkCommandBuffer cmd);
    friend class VkGraphCompiler;
    friend class VkGraphBuilder;
    friend class VkResourceAllocator;
    friend class VkGraph;
    friend class VkDiscardPool;
    void* vData__;
    void* iData__;
    VkDeviceSize vSize__;
    VkDeviceSize iSize__;
    VkBuffer vertexBuffer__;
    VkBuffer indexBuffer__;
    VkPipeline pipeline__;
    VkPipelineLayout pipelineLayout__;
    VkAccessFlagBits writeAccess__ = VK_ACCESS_TRANSFER_WRITE_BIT;
    VkPipelineStageFlagBits writePipelineState__ = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    std::vector<SubMesh> submeshes__;
    std::vector<VkDescriptorSet> descriptorSet__;
    VkBool32 dirty__;
  };


  class VkImageNode : public VkNode
  {
    public:
    VkFormat format__ = VK_FORMAT_R8G8B8A8_UNORM;
    VkImageType imageType__ = VK_IMAGE_TYPE_2D;
    uint32_t height__ = 0;
    uint32_t width__ = 0;
    VkImageAspectFlags aspectMask__;

    private:
    friend class VkGraph;
    friend class VkGraphCompiler;
    friend class VkGraphBuilder;
    friend class VkResourceAllocator;
    friend class VkDiscardPool;

    VkImage imageh__ = VK_NULL_HANDLE;
    VkImageView imageView__;
    uint32_t levelCount__ = 0;
    VkImageLayout currentLayout__ = VK_IMAGE_LAYOUT_UNDEFINED;
    VkImageLayout writeLayout__ = VK_IMAGE_LAYOUT_UNDEFINED;
    VkBool32 writen__ = false;
    uint32_t mipLevels__ = 0;
    VkImageUsageFlags usage__;
  };

  class VkTextureNode : public VkImageNode
  {
    friend class VkGraphCompiler;
    friend class VkGraphBuilder;
    friend class VkResourceAllocator;
    friend class VkDescriptorAllocator;
    void* data__;
    VkImage img__ = VK_NULL_HANDLE;
    VkImageView view__ = VK_NULL_HANDLE;
    VkSampler sampler = VK_NULL_HANDLE;
    VkDescriptorSet descriptor = VK_NULL_HANDLE;
    VkImageViewType imgViewType = VK_IMAGE_VIEW_TYPE_2D;
    VkFormat imgFormat = VK_FORMAT_R8G8B8A8_SRGB;
    VkImageLayout imgLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    uint32_t depth = 0;
    uint32_t mipLevels = 0;
    uint32_t arrayLevels = 0;
    VkDeviceSize imageOffset = 0;
    uint32_t descriptorArrayIndex = 0;
    uint32_t bindigIndex = 0;
    std::string name;
    bool ktx = false;
  };

  class VkShaderNode : public VkNode
  {
    VkShaderStageFlagBits stage;
    VkPipelineLayout layout;
    VkPipeline pipeline;
    std::vector<VkDescriptorSetLayout> descriptorLayouts;
    std::string entryPoint;
    std::vector<uint32_t> codeHash; // Shader module 캐싱용

    bool compiled = false; // pipeline 생성 여부
    bool dirty = false;    // 코드/파라미터 변경 여부
  };

  class VkPass
  {
    public:
    RenderPassType passType;
    std::vector<VkNode*> read__;
    std::vector<VkNode*> write__;
    std::function<void(VkCommandBuffer cmd)> execute = nullptr;
    std::optional<VkViewport> setViewPort_;
    std::optional<VkClearValue> clearValue_;
    std::optional<VkPolygonMode> polygonMode_;
    std::vector<VkImageMemoryBarrier> imageMemoryBarriers__;
    std::vector<VkBufferMemoryBarrier> bufferBarriers__;
    struct
    {
      std::vector<VkRenderingAttachmentInfo> colorAttachment__;
      std::optional<VkRenderingAttachmentInfo> depthAttachment__;
      std::optional<VkRenderingAttachmentInfo> stencilAttachment__;
      std::optional<VkViewport> viewport__;
      VkBool32 writen__;
      VkExtent2D renderingArea{};
      VkRect2D scissor;
      VkBool32 useDepthTest;
      VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL;
      PFN_vkCmdSetPolygonModeEXT vkCmdSetPolygonModeEXT;
      VkClearColorValue clearColor__;
    } passParameter__;
    void link();
    friend class VkGraphCompiler;
    friend class VkGraphBuilder;
    friend class VkResourceAllocator;
    friend class VkGraph;
    uint32_t linkCount;
    std::vector<VkPass*> dependency__ = {};
    std::vector<VkPass*> dependent__ = {};
    uint32_t passId__ = 0;
    bool culled = false;
    bool transitionPass = false;
  };
}


#endif //MYPROJECT_RENDER_GRAPH_NODE_HPP
