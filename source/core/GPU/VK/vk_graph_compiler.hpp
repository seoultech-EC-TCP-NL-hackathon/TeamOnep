#ifndef VK_GRAPH_COMPILER
#define VK_GRAPH_COMPILER

#include <unordered_map>
#include <queue>

#include "vk_common.hpp"
#include "vk_resource.hpp"
#include "vk_swapchain.hpp"
#include "vk_context.hpp"

namespace gpu{
  class VkGraphCompiler{
    friend class VkScheduler;

    VkGraphCompiler(VkContext *pCtxt);
    ~VkGraphCompiler() = default;
    void compileGraph();

  private:
    void allocate(VkResource *node);
    void maskingTimeline(VkResource *node);
    void compileGraph(VkPass *renderPass);

    void readSync(VkFrameAttachment *image);
    void readSync(VkHostBuffer *fBuffer);

    void insertResolve(VkFrameAttachment *image);
    void writeSync(VkFrameAttachment *image);
    void writeSync(VkHostBuffer *fBuffer);

    bool needBufferBarrier(uint32_t readPipeline,
                           uint32_t readAccessMask,
                           VkHostBuffer *readBuffer);
    uint32_t getResourceUsage(gpu::ResourceUsage usage);

    VkPipelineStageFlags decideReadPipeline(gpu::ResourceUsage readUsage);
    VkImageLayout decideReadImageLayout(gpu::ResourceUsage readUsage);
    VkAccessFlags decideReadAccessMask(gpu::ResourceUsage readUsage);

    std::function<void(VkCommandBuffer cmd)> buildBufferBarrier(VkAccessFlags srcAccessMask,
                                                                VkAccessFlags dstAccessMask,
                                                                VkPipelineStageFlags srcStageMask,
                                                                VkPipelineStageFlags dstStageMask,
                                                                VkHostBuffer &frameBuffer,
                                                                uint32_t srcQFamily,
                                                                uint32_t dstQFamily,
                                                                VkHostBuffer *buffer);

    std::function<void(VkCommandBuffer cmd)> buildImageBarrier(VkAccessFlags srcAccessMask,
                                                               VkAccessFlags dstAccessMask,
                                                               VkPipelineStageFlags srcStageMask,
                                                               VkPipelineStageFlags dstStageMask,
                                                               VkImageLayout srcImageLayout,
                                                               VkImageLayout dstImageLayout,
                                                               uint32_t currentQ,
                                                               uint32_t dstQ,
                                                               gpu::VkFrameAttachment *srcImage);

    std::function<void(VkCommandBuffer cmd)> buildBufferCopyToImage(gpu::VkHostBuffer *srcBuffer,
                                                                    gpu::VkFrameAttachment *dstImage);

    std::function<void(VkCommandBuffer cmd)> buildBufferCopyToBuffer(gpu::VkHostBuffer *srcBuffer,
                                                                     gpu::VkHostBuffer *dstBuffer);
    VkBool32 renderBegin = false;
    gpu::VkContext *pCtxt;
    std::unordered_set<VkResource*> frameNodes_;
  };
}


// void updateRsc(VkResource *node);

#endif