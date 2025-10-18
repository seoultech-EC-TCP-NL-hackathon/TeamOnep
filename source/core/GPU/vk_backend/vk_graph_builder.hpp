//
// Created by ljh on 25. 10. 10..
//

#ifndef MYPROJECT_VK_GRAPH_BUILDER_HPP
#define MYPROJECT_VK_GRAPH_BUILDER_HPP
#include <memory>
#include <unordered_map>
#include "util/unique.hpp"
#include "vk_node.hpp"
//#include "vk_sampler_builder"


///todo:
/// builder test :
/// declare node and pass -> subpass barrier test first
/// ->after barrier test, must test batch control
/// ->life time and resource scheduler
namespace gpu
{
  class VkContext;

  class VkGraphBuilder{
    friend class VkScheduler ;
  public:
    VkGraphBuilder(VkContext* pCtxt);
    ~VkGraphBuilder();
    VkPassId addPass(std::unique_ptr<VkPass>& pass);
    void addResource(VkPassId passId,
                     VkNodeId read,
                     VkNodeId write);

    void addReadResource(VkPassId passId,
                     VkNodeId read);
    void addWriteResource(VkPassId passId, VkNodeId write);

    void uploadCopyPass(VkNode* read,
                        VkNode* write);

    void buildSwapchainImage();
    VkNodeId buildBufferHandle(std::unique_ptr<VkBufferNode>& VkFBuffer);
    VkNodeId buildMeshBuffer(std::unique_ptr<VkMeshBuffer>& buffer);
    VkNodeId buildImageHandle(std::unique_ptr<VkImageNode>& image);
    VkNodeId getSwapchainImage();
    VkNodeId buildTexture(VkTextureNode* texture);
    VkNodeId buildBatch();

  private:
    std::vector<VkDependencyFlags> dependency_;
    std::vector<std::unique_ptr<gpu::VkNode>> nodes_;
    std::vector<std::unique_ptr<gpu::VkPass>> passes_;
    std::vector<VkNodeId> swapchainHandle;
    VkNodeId nodeId_ = 0;
    VkPassId passId_ = 0;
    VkContext* pCtxt_;

    inline void flag();
  };
}

#endif //MYPROJECT_VK_GRAPH_BUILDER_HPP
