//
// Created by ljh on 25. 10. 10..
//

#ifndef MYPROJECT_VK_GRAPH_BUILDER_HPP
#define MYPROJECT_VK_GRAPH_BUILDER_HPP
#include <memory>
#include <unordered_map>

#include "IBuffer.hpp"
#include "util/unique.hpp"
#include "vk_resource.hpp"
#include "vk_host_buffer.h"
#include "vk_texture.hpp"
//#include "vk_sampler_builder"

///todo:
/// builder test :
/// declare node and pass -> subpass barrier test first
/// ->after barrier test, must test batch control
/// ->life time and resource scheduler
namespace gpu
{
  class VkContext;
  extern VkContext* ctx__;
  using SwapchainHandle = uint32_t;

  class VkGraphBuilder
  {
    friend class VkScheduler;

    public:
    VkGraphBuilder();
    ~VkGraphBuilder();
    static VkPassId addPass(std::unique_ptr<VkPass>& pass);
    static void addResource(VkPassId passId,
                     VkResourceId read,
                     VkResourceId write);

    static void addReadResource(VkPassId passId, VkResourceId read);
    static void addWriteResource(VkPassId passId, VkResourceId write);
    static void uploadCopyPass(VkResource* read,
                        VkResource* write);

    static void buildSwapchainImage();
    static VkResourceId buildBufferHandle(std::unique_ptr<VkHostBuffer>& VkFBuffer);
    static VkHostBuffer buildHostBuffer(VkDeviceSize size, BufferType bufferType);
    static gpu::VkMeshBuffer* registerMeshBuffer(std::unique_ptr<VkMeshBuffer>& buffer);
    static VkResourceId registerImage(std::unique_ptr<VkFrameAttachment>& image);
    static VkResourceId getSwapchainImage();
    static VkResourceId buildTexture(VkTexture* texture);
    static VkResourceId buildBatch();
    static VkFrameAttachment* buildDepthAttachment();

    private:
    static void flag();
  };
}

#endif //MYPROJECT_VK_GRAPH_BUILDER_HPP
