#include <memory>
#include <spdlog/spdlog.h>
#include "util/unique.hpp"
#include "vk_swapchain.hpp"
#include "vk_context.hpp"
#include "vk_graph_builder.hpp"
#include "../IBuffer.hpp"

///same with backend builder
///
namespace gpu
{
  //todo::change to static function
  VkGraphBuilder::VkGraphBuilder()
  {
  }

  //void VkGraphBuilder::uploadCopyPass(VkResource* read, VkResource* write)
  //{
  //  VkPass copyPass{
  //    .passType = RenderPassType::COPY_PASS,
  //    .read__ = {},
  //    .write__ = {},
  //    .execute =
  //    [this,
  //      src,
  //      dst,
  //      srcOffset,
  //      dstOffset,
  //      size]
  //  (VkCommandBuffer cmd)
  //    {
  //      VkBufferCopy region{};
  //      region.srcOffset = 0;
  //      region.dstOffset = 0;
  //      region.size = size;
  //      vkCmdCopyBuffer(cmd, src, dst, 1, &region);
  //
  //      VkBufferMemoryBarrier barrier{
  //        .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
  //        .srcAccessMask = VK_ACCESS_NONE_KHR,
  //        .dstAccessMask = VK_ACCESS_NONE_KHR,
  //        .srcQueueFamilyIndex = pCtxt->graphicsFamailyIdx__,
  //        .dstQueueFamilyIndex = pCtxt->graphicsFamailyIdx__,
  //        .buffer = dst,
  //        .size = size,
  //      };
  //      barrier.offset = dstOffset;
  //      vkCmdPipelineBarrier(
  //                           cmd,
  //                           VK_PIPELINE_STAGE_TRANSFER_BIT,
  //                           VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
  //                           0,
  //                           0,
  //                           nullptr,
  //                           1,
  //                           &barrier,
  //                           0,
  //                           nullptr
  //                          );
  //    }
  //  };
  //  copyPass.transitionPass = true;
  //  pCtxt->compiledPass.push_back(copyPass);
  //}
  VkDeviceSize alignUp(VkDeviceSize v, VkDeviceSize a)
  {
    return (v + (a - 1)) & ~(a - 1);
  }

  VkGraphBuilder::~VkGraphBuilder() = default;

  void VkGraphBuilder::flag()
  {
    ctx__->dirty_ = true;
  }

  //tset
  VkPassId gpu::VkGraphBuilder::addPass(mns::uptr<VkPass>& pass_)
  {
    mns::uptr<VkPass> pass = std::move(pass_);
    pass->passId__ = ctx__->passId_;
    ctx__->passHash_[ctx__->passId_] = pass.get();
    ctx__->uploadedPass.push_back(pass.get());
    ctx__->passes_.push_back(std::move(pass));
    flag();
    return ctx__->passId_++;
  }

  void gpu::VkGraphBuilder::addResource(VkPassId passId, VkResourceId read, VkResourceId write)
  {
    VkPass* pass = ctx__->passHash_[passId];
    VkResource* writeNode = ctx__->nodeHash_[write];
    VkResource* readNode = ctx__->nodeHash_[read];
    writeNode->lastWriter__ = nullptr;
    readNode->lastWriter__ = nullptr;

    pass->write__.push_back(writeNode);
    pass->read__.push_back(readNode);

    flag();
  }

  void gpu::VkGraphBuilder::addReadResource(VkPassId passId, VkResourceId read)
  {
    VkPass* pass = ctx__->passHash_[passId];
    VkResource* readNode = ctx__->nodeHash_[read];
    readNode->lastWriter__ = nullptr;
    pass->read__.push_back(readNode);
    flag();
  }

  void gpu::VkGraphBuilder::addWriteResource(VkPassId passId, VkResourceId write)
  {
    VkPass* pass = ctx__->passHash_[passId];
    VkResource* writeNode = ctx__->nodeHash_[write];
    writeNode->lastWriter__ = nullptr;
    pass->write__.push_back(writeNode);
    flag();
  }

  VkResourceId gpu::VkGraphBuilder::buildBufferHandle(mns::uptr<VkHostBuffer>& buffer)
  {
    buffer->nodeId_ = ctx__->nodeId_++;
    ctx__->nodeHash_[buffer->nodeId_] = buffer.get();

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
    ctx__->nodes_.push_back(std::move(buffer));
    flag();
    return ctx__->nodeId_ - 1;
  }

  VkHostBuffer VkGraphBuilder::buildHostBuffer(VkDeviceSize size, BufferType bufferType)
  {
    VkBufferUsageFlags flags;
    switch (bufferType)
    {
      case BufferType::VERTEX:
        flags = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
          VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        break;
      case BufferType::INDEX:
        flags = VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
          VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        break;
      case BufferType::UNIFORM:
        flags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        size = alignUp(size,
                       ctx__->deviceProperties__.limits.minUniformBufferOffsetAlignment);
        break;
      case BufferType::STORAGE:
        flags = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
          VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        size = alignUp(size,
                       ctx__->deviceProperties__.limits.minStorageBufferOffsetAlignment);
        break;
      case BufferType::STAGE:
        flags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT |
          VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    }

    VkHostBuffer hostBuffer;
    hostBuffer.size_ = size;
    hostBuffer.usage__ = flags;
    hostBuffer.nodeId_ = ctx__->nodeId_;
    hostBuffer.bufferh_ = ctx__->pResourceAllocator->buildBufferHandle(size, flags);
    hostBuffer.allocation__ = ctx__->pResourceAllocator->mBindBuffer(hostBuffer.bufferh_,
                                                                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    hostBuffer.allocated__ = true;
    return hostBuffer;
  }


  gpu::VkMeshBuffer* gpu::VkGraphBuilder::registerMeshBuffer(mns::uptr<gpu::VkMeshBuffer>& buffer)
  {
    uint32_t id = ctx__->nodeId_++;
    buffer->nodeId_ = id;
    ctx__->nodeHash_[buffer->nodeId_] = buffer.get();
    buffer->type_ = gpu::ResourceType::MESH;
    buffer->mFlags_ = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    buffer->usage_ = gpu::ResourceUsage::MESH_BUFFER;
    buffer->lastWriter__ = nullptr;
    auto ptr = buffer.get();
    ctx__->nodes_.push_back(std::move(buffer));
    flag();
    return ptr;
  }

  VkResourceId VkGraphBuilder::registerImage(mns::uptr<VkFrameAttachment>& image)
  {
    uint32_t id = ctx__->nodeId_;
    image->nodeId_ = ctx__->nodeId_++;
    ctx__->nodeHash_[image->nodeId_] = image.get();
    ctx__->nodes_.push_back(std::move(image));
    return id;
    flag();
  }

  VkResourceId VkGraphBuilder::getSwapchainImage()
  {
    if (ctx__->pSwapChainContext->broked__)
    {
      buildSwapchainImage();
    }
    return ctx__->swapchainHandle[ctx__->renderingContext.currentFrame__];
  }

  void VkGraphBuilder::buildSwapchainImage()
  {
    for (uint32_t i = 0; i < ctx__->pSwapChainContext->img__.size(); ++i)
    {
      mns::uptr<VkFrameAttachment> swapchainImage = std::make_unique<VkFrameAttachment>();
      swapchainImage->allocated__ = true;
      swapchainImage->format__ = ctx__->pSwapChainContext->imgFormat__;
      swapchainImage->imageh__ = ctx__->pSwapChainContext->img__[i];
      swapchainImage->imageView__ = ctx__->pSwapChainContext->imgView__[i];
      swapchainImage->levelCount__ = 1;
      swapchainImage->type_ = ResourceType::IMAGE;
      swapchainImage->usage_ = ResourceUsage::G_BUFFER;
      swapchainImage->lifetime = VkResourceLifetime::PERSISTENT;
      swapchainImage->aspectMask__ = VK_IMAGE_ASPECT_COLOR_BIT;
      swapchainImage->lastWriter__ = nullptr;
      swapchainImage->height__ = ctx__->pSwapChainContext->extent__.height;
      swapchainImage->width__ = ctx__->pSwapChainContext->extent__.width;
      swapchainImage->nodeId_ = ctx__->nodeId_++;
      swapchainImage->currentLayout__ = VK_IMAGE_LAYOUT_UNDEFINED;
      swapchainImage->currentPipeline__ = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
      swapchainImage->currentAccessMask__ = VK_ACCESS_NONE;
      ctx__->nodeHash_[swapchainImage->nodeId_] = swapchainImage.get();
      uint32_t id = swapchainImage->nodeId_;
      ctx__->nodes_.push_back(std::move(swapchainImage));
      ctx__->swapchainHandle.push_back(id);
      flag();
    }
  }

  gpu::VkFrameAttachment* VkGraphBuilder::buildDepthAttachment()
  {
    mns::uptr<gpu::VkFrameAttachment> depth = mns::mUptr<gpu::VkFrameAttachment>();
    depth->type_ = gpu::ResourceType::IMAGE;
    depth->usage_ = gpu::ResourceUsage::DEPTH_STENCIL_ATTACHMENT;
    depth->aspectMask__ = VK_IMAGE_ASPECT_DEPTH_BIT;
    depth->format__ = VK_FORMAT_D32_SFLOAT;
    depth->height__ = gpu::ctx__->pSwapChainContext->extent__.height;
    depth->width__ = gpu::ctx__->pSwapChainContext->extent__.width;
    depth->mSpace_ = gpu::MemorySpace::DEVICE_LOCAL;
    depth->lifetime = gpu::VkResourceLifetime::FRAME;
    depth->mipLevels__ = 1;
    depth->levelCount__ = 1;
    depth->descriptorSet__ = VK_NULL_HANDLE;
    depth->bindigIndex = 2;
    depth->descriptorArrayIndex = 0;
    auto ptr = depth.get();
    gpu::ctx__->graphBuilder.registerImage(depth);
    return ptr;
  }
}
