#ifndef VK_GRAPH_COMPILER
#define VK_GRAPH_COMPILER

#include <unordered_map>
#include <queue>

#include "vk_common.hpp"
#include "vk_node.hpp"
#include "vk_swapchain.hpp"
#include "vk_context.hpp"

namespace gpu{
  class VkGraphCompiler{
    friend class VkScheduler;

    VkGraphCompiler(VkContext *pCtxt);
    ~VkGraphCompiler() = default;
    void compileGraph();

  private:
    void allocate(VkNode *node);
    void maskingTimeline(VkNode *node);
    void compileGraph(VkPass *renderPass);

    void readSync(VkImageNode *image);
    void readSync(VkBufferNode *fBuffer);

    void insertResolve(VkImageNode *image);
    void writeSync(VkImageNode *image);
    void writeSync(VkBufferNode *fBuffer);


    bool needBufferBarrier(uint32_t readPipeline,
                           uint32_t readAccessMask,
                           VkBufferNode *readBuffer);
    uint32_t getResourceUsage(gpu::ResourceUsage usage);

    VkPipelineStageFlags decideReadPipeline(gpu::ResourceUsage readUsage);
    VkImageLayout decideReadImageLayout(gpu::ResourceUsage readUsage);
    VkAccessFlags decideReadAccessMask(gpu::ResourceUsage readUsage);

    std::function<void(VkCommandBuffer cmd)> buildBufferBarrier(VkAccessFlags srcAccessMask,
                                                                VkAccessFlags dstAccessMask,
                                                                VkPipelineStageFlags srcStageMask,
                                                                VkPipelineStageFlags dstStageMask,
                                                                VkBufferNode &frameBuffer,
                                                                uint32_t srcQFamily,
                                                                uint32_t dstQFamily,
                                                                VkBufferNode *buffer);

    std::function<void(VkCommandBuffer cmd)> buildImageBarrier(VkAccessFlags srcAccessMask,
                                                               VkAccessFlags dstAccessMask,
                                                               VkPipelineStageFlags srcStageMask,
                                                               VkPipelineStageFlags dstStageMask,
                                                               VkImageLayout srcImageLayout,
                                                               VkImageLayout dstImageLayout,
                                                               uint32_t currentQ,
                                                               uint32_t dstQ,
                                                               gpu::VkImageNode *srcImage);

    std::function<void(VkCommandBuffer cmd)> buildBufferCopyToImage(gpu::VkBufferNode *srcBuffer,
                                                                    gpu::VkImageNode *dstImage);

    std::function<void(VkCommandBuffer cmd)> buildBufferCopyToBuffer(gpu::VkBufferNode *srcBuffer,
                                                                     gpu::VkBufferNode *dstBuffer);
    VkBool32 renderBegin = false;
    gpu::VkContext *pCtxt;
    std::unordered_set<VkNode*> frameNodes_;
  };
}


// void updateRsc(VkNode *node);

#endif