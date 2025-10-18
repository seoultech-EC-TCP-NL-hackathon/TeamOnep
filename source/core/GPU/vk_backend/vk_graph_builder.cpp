#include <memory>
#include <spdlog/spdlog.h>
#include "util/unique.hpp"
#include "vk_swapchain.hpp"
#include "vk_context.hpp"
#include "vk_graph_builder.hpp"

///same with backend builder
///
namespace gpu
{
  VkGraphBuilder::VkGraphBuilder(VkContext* pCtxt) :
    pCtxt_(pCtxt)
  {
    buildSwapchainImage();
  }

  VkGraphBuilder::~VkGraphBuilder() = default;

  void VkGraphBuilder::flag()
  {
    pCtxt_->dirty_ = true;
  }

  //tset
  VkPassId gpu::VkGraphBuilder::addPass(mns::uptr<VkPass>& pass_)
  {
    mns::uptr<VkPass> pass = std::move(pass_);
    pass->passId__ = passId_++;
    pCtxt_->passHash_[passId_] = pass.get();
    pCtxt_->uploadedPass.push_back(pass.get());
    passes_.push_back(std::move(pass));
    flag();
    return passId_;
  }

  void gpu::VkGraphBuilder::addResource(VkPassId passId, VkNodeId read, VkNodeId write)
  {
    VkPass* pass = pCtxt_->passHash_[passId];
    VkNode* writeNode = pCtxt_->nodeHash_[write];
    VkNode* readNode = pCtxt_->nodeHash_[read];
    pass->write__.push_back(writeNode);
    pass->read__.push_back(readNode);

    flag();
  }

  void gpu::VkGraphBuilder::addReadResource(VkPassId passId, VkNodeId read)
  {
    VkPass* pass = pCtxt_->passHash_[passId];
    VkNode* readNode = pCtxt_->nodeHash_[read];
    pass->read__.push_back(readNode);
    flag();
  }

  void gpu::VkGraphBuilder::addWriteResource(VkPassId passId, VkNodeId write)
  {
    VkPass* pass = pCtxt_->passHash_[passId];
    VkNode* writeNode = pCtxt_->nodeHash_[write];
    pass->write__.push_back(writeNode);
    flag();
  }

  VkNodeId gpu::VkGraphBuilder::buildBufferHandle(mns::uptr<VkBufferNode>& buffer)
  {
    buffer->nodeId_ = nodeId_++;
    pCtxt_->nodeHash_[buffer->nodeId_] = buffer.get();

    buffer->mFlags_ = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
      VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    //if (buffer->mSpace_ == MemorySpace::DEVICE_LOCAL)
    //{
    //  buffer->mFlags_                         = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    //  mns::uptr<GPU::VkFBuffer> staging = std::make_unique<GPU::VkFBuffer>();
    //  VkPassId copyPass                       = GPU::VkGraphBuilder::addPass();
    //  addResource(copyPass, staging->nodeId_, buffer->nodeId_);
    //  pCtxt->nodeHash_[staging->nodeId_] = staging.get();
    //  //(std::move(staging));
    //}
    nodes_.push_back(std::move(buffer));
    flag();
    return nodeId_ - 1;
  }

  VkNodeId gpu::VkGraphBuilder::buildMeshBuffer(mns::uptr<gpu::VkMeshBuffer>& buffer)
  {
    uint32_t id = nodeId_++;
    buffer->nodeId_ = id;
    pCtxt_->nodeHash_[buffer->nodeId_] = buffer.get();
    buffer->type_ = gpu::ResourceType::MESH;
    buffer->mFlags_ = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    buffer->usage_ = gpu::ResourceUsage::MESH_BUFFER;
    buffer->nodeName_ = "buffer test";
    buffer->lastWriter__ = nullptr;
    nodes_.push_back(std::move(buffer));
    flag();
    return id;
  }

  // format isn't determine
  VkNodeId VkGraphBuilder::buildImageHandle(mns::uptr<VkImageNode>& image)
  {
    uint32_t id = nodeId_;
    image->nodeId_ = nodeId_++;
    pCtxt_->nodeHash_[image->nodeId_] = image.get();
    nodes_.push_back(std::move(image));
    return id;
    flag();
  }

  VkNodeId VkGraphBuilder::getSwapchainImage()
  {
    if (pCtxt_->pSwapChainContext->broked__)
    {
      buildSwapchainImage();
    }
    return swapchainHandle[pCtxt_->renderingContext.currentFrame__];
  }
  void VkGraphBuilder::buildSwapchainImage()
  {
    for (uint32_t i = 0; i < pCtxt_->pSwapChainContext->img__.size(); ++i)
    {
      mns::uptr<VkImageNode> swapchainImage = std::make_unique<VkImageNode>();
      swapchainImage->allocated__ = true;
      swapchainImage->binded__ = true;
      swapchainImage->format__ = pCtxt_->pSwapChainContext->imgFormat__;
      swapchainImage->imageh__ = pCtxt_->pSwapChainContext->img__[i];
      swapchainImage->imageView__ = pCtxt_->pSwapChainContext->imgView__[i];
      swapchainImage->levelCount__ = 1;
      swapchainImage->type_ = ResourceType::IMAGE;
      swapchainImage->usage_ = ResourceUsage::G_BUFFER;
      swapchainImage->lifetime = VkResourceLifetime::PERSISTENT;
      swapchainImage->aspectMask__ = VK_IMAGE_ASPECT_COLOR_BIT;
      swapchainImage->height__ = pCtxt_->pSwapChainContext->extent__.height;
      swapchainImage->width__ = pCtxt_->pSwapChainContext->extent__.width;
      swapchainImage->nodeId_ = nodeId_++;
      swapchainImage->currentLayout__ = VK_IMAGE_LAYOUT_UNDEFINED;
      pCtxt_->nodeHash_[swapchainImage->nodeId_] = swapchainImage.get();
      uint32_t id = swapchainImage->nodeId_;
      nodes_.push_back(std::move(swapchainImage));
      swapchainHandle.push_back(id);
      flag();
    }
  }
}
