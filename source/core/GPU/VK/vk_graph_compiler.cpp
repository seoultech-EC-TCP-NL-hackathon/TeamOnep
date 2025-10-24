#include <filesystem>
#include <vulkan/vulkan.h>
#include <sys/stat.h>
#include <unordered_set>
#include <deque>
#include <algorithm>
#include "vk_context.hpp"
#include "vk_discard_pool.hpp"
#include "vk_resource_allocator.hpp"
#include "vk_graph_compiler.hpp"

namespace gpu
{
  // expect base layout
  //: ex (g-buffer -> shadow -> lighting) declare order
  //  tracking last writer -> if need barrier, insert
  //
  //
  //todo : if need,
  //       insert write->reader dependency
  VkGraphCompiler::VkGraphCompiler(VkContext* pCtxt) :
    pCtxt(pCtxt)
  {
  }

  void VkGraphCompiler::compileGraph()
  {
    pCtxt->compiledPass.clear();
    VkResource* node = pCtxt->nodeHash_[pCtxt->graphBuilder.getSwapchainImage()];
    VkFrameAttachment* swapchain = reinterpret_cast<VkFrameAttachment*>(node);
    swapchain->lastWriter__ = nullptr;

    std::vector<VkPass*>& passes = pCtxt->uploadedPass;
    for (auto* pass : passes)
    {
      for (auto* read__ : pass->read__)
      {
        read__->currentAccessMask__ = VK_ACCESS_NONE;
        read__->currentPipeline__ = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        if (read__->lastWriter__ != nullptr && pass != read__->lastWriter__)
        {
          pass->dependency__.insert(read__->lastWriter__);
          read__->lastWriter__->dependent__.insert(pass);
        }
      }
      for (auto* write__ : pass->write__)
      {
        if (write__->lastWriter__ != nullptr && pass != write__->lastWriter__)
        {
          pass->dependency__.insert(write__->lastWriter__);
          write__->lastWriter__->dependent__.insert(pass);
        }
        maskingTimeline(write__);
        write__->lastWriter__ = pass;
      }
    }
    ///todo:
    /// find Last Comsummer and trimming
    std::deque<VkPass*> ready;
    for (auto* pass : pCtxt->uploadedPass)
    {
      pass->linkCount = pass->dependency__.size();
      if (pass->dependency__.size() == 0)
      {
        pass->dependency__.clear();
        ready.push_back(pass);
      }
    }
    while (!ready.empty())
    {
      VkPass* pass = ready.front();
      ready.pop_front();
      compileGraph(pass);
      for (auto* postPass : pass->dependent__)
      {
        postPass->linkCount--;
        if (postPass->linkCount == 0)
        {
          ready.push_back(postPass);
        }
      }
      pass->dependent__.clear();
    }
    //std::vector<VkResource*> frameResources;
    //for (auto& node : frameNodes_)
    //{
    //  frameResources.push_back(node);
    //}

    VkPass barrierPass;
    barrierPass.passType = VkRenderPassType::BARRIER_PASS;
    barrierPass.execute = buildImageBarrier(swapchain->currentAccessMask__,
                                            0,
                                            swapchain->currentPipeline__,
                                            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                            swapchain->currentLayout__,
                                            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                                            pCtxt->graphicsFamailyIdx__,
                                            pCtxt->graphicsFamailyIdx__,
                                            swapchain);
    swapchain->currentLayout__ = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    swapchain->currentAccessMask__ = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    swapchain->currentPipeline__ = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    pCtxt->compiledPass.push_back(barrierPass);
    //pCtxt->pDiscardPool->registerResource(frameResources);
    pCtxt->uploadedPass.clear();
  }

  ///resource usage-> allocate
  /// memory bind(triming -> allocate)
  void VkGraphCompiler::allocate(VkResource* _read)
  {
    if (!_read->allocated__)
    {
      if (_read->type_ == gpu::ResourceType::IMAGE)
      {
        VkFrameAttachment* frameImage = reinterpret_cast<gpu::VkFrameAttachment*>(_read);
        frameImage->usage__ = getResourceUsage(frameImage->usage_);
        pCtxt->pResourceAllocator->buildImageNode(frameImage);
      }
      if (_read->type_ == ResourceType::BUFFER)
      {
        VkHostBuffer* frameBuffer = reinterpret_cast<gpu::VkHostBuffer*>(_read);
        frameBuffer->usage__ = getResourceUsage(frameBuffer->usage_);
        frameBuffer->bufferh_ = pCtxt->pResourceAllocator->buildBufferHandle(frameBuffer->size_,
                                                                             frameBuffer->usage__);
      }
      if (_read->type_ == ResourceType::MESH)
      {
        VkMeshBuffer* mesh = reinterpret_cast<gpu::VkMeshBuffer*>(_read);
        pCtxt->pResourceAllocator->buildMeshNode(mesh);
      }
    }
    _read->allocated__ = true;
  }


  void VkGraphCompiler::maskingTimeline(VkResource* node)
  {
    if (node->type_ == gpu::ResourceType::IMAGE)
    {
      VkFrameAttachment* frameImage = reinterpret_cast<gpu::VkFrameAttachment*>(node);
      if (frameImage->usage_ & (gpu::ResourceUsage::G_BUFFER |
        gpu::ResourceUsage::LIGHTNING_BUFFER))
      {
        frameImage->writeAccessMask__ = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        frameImage->writePipeline__ = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        frameImage->writeLayout__ = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
      }
      if (frameImage->usage_ & (gpu::ResourceUsage::DEPTH_STENCIL_ATTACHMENT |
        gpu::ResourceUsage::SHADOW_BUFFER))
      {
        frameImage->writeAccessMask__ = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT |
          VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
        frameImage->writePipeline__ = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT |
          VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        frameImage->writeLayout__ = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
      }
      if (frameImage->usage_ & gpu::ResourceUsage::TEXTURE)
      {
        frameImage->writeAccessMask__ = VK_ACCESS_TRANSFER_WRITE_BIT;
        frameImage->writePipeline__ = VK_PIPELINE_STAGE_TRANSFER_BIT;
        frameImage->writeLayout__ = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
      }
    }
    if (node->type_ == gpu::ResourceType::BUFFER)
    {
      if (node->usage_ & (gpu::ResourceUsage::MESH_BUFFER |
        gpu::ResourceUsage::VERTEX_BUFFER))
      {
        VkMeshBuffer* buffer = reinterpret_cast<gpu::VkMeshBuffer*>(node);
        buffer->writeAccessMask__ = VK_ACCESS_TRANSFER_WRITE_BIT;
        buffer->writePipeline__ = VK_PIPELINE_STAGE_TRANSFER_BIT;
      }
      if (node->usage_ & (gpu::ResourceUsage::UNIFORM_BUFFER |
        gpu::ResourceUsage::SHADER_STORAGE_BUFFER))
      {
        //todo:
        //imple with shader
      }
    }
  }

  VkImageLayout gpu::VkGraphCompiler::decideReadImageLayout(gpu::ResourceUsage usage)
  {
    uint32_t usage_ = static_cast<uint32_t>(usage);
    if (usage & (gpu::ResourceUsage::G_BUFFER |
      gpu::ResourceUsage::LIGHTNING_BUFFER |
      gpu::ResourceUsage::FORWARD |
      gpu::ResourceUsage::POST_PROCESS))
    {
      return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    }
    if (usage & (gpu::ResourceUsage::DEPTH_STENCIL_ATTACHMENT |
      gpu::ResourceUsage::SHADOW_BUFFER))
    {
      return VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL;
    }
    if (usage & (gpu::ResourceUsage::TRANSFER |
      gpu::ResourceUsage::TEXTURE))
    {
      return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    }
    throw std::runtime_error("unsupported resource usage");
  }

  uint32_t gpu::VkGraphCompiler::getResourceUsage(gpu::ResourceUsage usage)
  {
    uint32_t usage_ = static_cast<uint32_t>(usage);
    if (usage_ & (gpu::ResourceUsage::MESH_BUFFER |
      gpu::ResourceUsage::VERTEX_BUFFER))
    {
      return static_cast<VkBufferUsageFlags>(
        static_cast<uint32_t>(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT) |
        static_cast<uint32_t>(VK_BUFFER_USAGE_INDEX_BUFFER_BIT) |
        static_cast<uint32_t>(VK_BUFFER_USAGE_TRANSFER_DST_BIT)
      );
    }
    if (usage_ & (gpu::ResourceUsage::DEPTH_STENCIL_ATTACHMENT |
      gpu::ResourceUsage::SHADOW_BUFFER))
    {
      return VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    }
    if (usage_ & (gpu::ResourceUsage::G_BUFFER |
      ResourceUsage::POST_PROCESS |
      ResourceUsage::FORWARD |
      ResourceUsage::LIGHTNING_BUFFER))
    {
      return VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    }
    if (usage_ & (gpu::ResourceUsage::UNIFORM_BUFFER))
    {
      return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT |
        VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    }
    if (usage_ & (gpu::ResourceUsage::SHADER_STORAGE_BUFFER))
    {
      return VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
        VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    }
    if (usage_ & (gpu::ResourceUsage::TEXTURE))
    {
      return VK_IMAGE_USAGE_TRANSFER_DST_BIT |
        VK_IMAGE_USAGE_SAMPLED_BIT;
    }
    throw std::runtime_error("unsupported resource usage");
  }

  uint32_t VkGraphCompiler::decideReadPipeline(gpu::ResourceUsage readUsage)
  {
    if (readUsage & (gpu::ResourceUsage::MESH_BUFFER |
      gpu::ResourceUsage::VERTEX_BUFFER))
    {
      return VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
    }
    if (readUsage & (gpu::ResourceUsage::DEPTH_STENCIL_ATTACHMENT |
      gpu::ResourceUsage::SHADOW_BUFFER))
    {
      return VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    }
    if (readUsage & (gpu::ResourceUsage::G_BUFFER |
      ResourceUsage::POST_PROCESS |
      ResourceUsage::FORWARD |
      ResourceUsage::LIGHTNING_BUFFER))
    {
      return VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }

    if (readUsage & (gpu::ResourceUsage::UNIFORM_BUFFER |
      gpu::ResourceUsage::SHADER_STORAGE_BUFFER))
    {
      return VK_PIPELINE_STAGE_VERTEX_INPUT_BIT |
        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    if (readUsage & (gpu::ResourceUsage::TEXTURE))
    {
      return VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    throw std::runtime_error("unsupported resource usage");
  }


  uint32_t gpu::VkGraphCompiler::decideReadAccessMask(gpu::ResourceUsage readUsage)
  {
    if (readUsage & (gpu::ResourceUsage::MESH_BUFFER |
      gpu::ResourceUsage::VERTEX_BUFFER))
    {
      return VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
    }


    if (readUsage & (gpu::ResourceUsage::DEPTH_STENCIL_ATTACHMENT |
      gpu::ResourceUsage::SHADOW_BUFFER))
    {
      return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
    }
    if (readUsage & (gpu::ResourceUsage::LIGHTNING_BUFFER |
      gpu::ResourceUsage::G_BUFFER |
      gpu::ResourceUsage::TEXTURE |
      gpu::ResourceUsage::POST_PROCESS))
    {
      return VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
    }

    if (readUsage & (gpu::ResourceUsage::MESH_BUFFER |
      gpu::ResourceUsage::VERTEX_BUFFER))
    {
      return VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
    }

    throw std::runtime_error("unsupported resource usage");
  }

  void gpu::VkGraphCompiler::compileGraph(VkPass* renderPass)
  {
    for (auto* _read : renderPass->read__)
    {
      allocate(_read);
      if (_read->type_ == gpu::ResourceType::IMAGE)
      {
        gpu::VkFrameAttachment* readImage = reinterpret_cast<gpu::VkFrameAttachment*>(_read);
        readSync(readImage);
        insertResolve(readImage);
        if (readImage->descriptorSet__ == VK_NULL_HANDLE)
        {
          readImage->descriptorSet__ = pCtxt->pDescriptorAllocator->allocate(pCtxt->renderingContext.currentFrame__);
          pCtxt ->pDescriptorAllocator->uploadBindlessTextureSet(readImage);
        }
      }
      if (_read->type_ == gpu::ResourceType::BUFFER)
      {
        VkHostBuffer* buffer = reinterpret_cast<VkHostBuffer*>(_read);
        readSync(buffer);
      }
    }
    for (auto* write : renderPass->write__)
    {
      if ((write->allocated__) == false) allocate(write);
      if (write->type_ == gpu::ResourceType::IMAGE)
      {
        gpu::VkFrameAttachment* frameImage = reinterpret_cast<gpu::VkFrameAttachment*>(write);
        writeSync(frameImage);
        if (frameImage->usage_ & (gpu::ResourceUsage::G_BUFFER |
          gpu::ResourceUsage::LIGHTNING_BUFFER))
        {
          if (!renderPass->passParameter__.writen__)
          {
            VkRenderingAttachmentInfo colorAttachment{
              .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
              .imageView = frameImage->imageView__,
              .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
              .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
              .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            };
            colorAttachment.clearValue.color = renderPass->passParameter__.clearColor__;
            if (frameImage->writen__)
            {
              colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
            }
            frameImage->writen__ = true;
            renderPass->passParameter__.colorAttachment__.push_back(colorAttachment);
          }
        }

        if (frameImage->usage_ & (gpu::ResourceUsage::DEPTH_STENCIL_ATTACHMENT |
          gpu::ResourceUsage::SHADOW_BUFFER))
        {
          if (!renderPass->passParameter__.writen__)
          {
            VkRenderingAttachmentInfo depthAttachment{};
            depthAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
            depthAttachment.imageView = frameImage->imageView__;
            depthAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL_KHR;
            depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            depthAttachment.clearValue.color = renderPass->passParameter__.clearColor__;
            depthAttachment.clearValue.depthStencil = {1.0f, 0};
            renderPass->passParameter__.depthAttachment__ = depthAttachment;
          }
        }
        if (write->type_ == gpu::ResourceType::BUFFER)
        {
          VkHostBuffer* buffer = reinterpret_cast<VkHostBuffer*>(write);
          writeSync(buffer);
        }
      }
    }
    pCtxt->pDescriptorAllocator->update();
    if (renderPass->execute != nullptr)
    {
      pCtxt->compiledPass.push_back(*renderPass);
    }
  }


  void VkGraphCompiler::readSync(VkFrameAttachment* image)
  {
    VkImageLayout readLayout = decideReadImageLayout(image->usage_);
    VkAccessFlags readAccessMask = decideReadAccessMask(image->usage_);
    VkPipelineStageFlags readPipeline = decideReadPipeline(image->usage_);

    if (image->currentLayout__ != readLayout ||
      image->currentAccessMask__ != readAccessMask ||
      image->currentPipeline__ != readPipeline)
    {
      VkPass barrierPass;
      barrierPass.passType = VkRenderPassType::BARRIER_PASS;
      barrierPass.execute = buildImageBarrier(image->currentAccessMask__,
                                              readAccessMask,
                                              image->currentPipeline__,
                                              readPipeline,
                                              image->currentLayout__,
                                              readLayout,
                                              pCtxt->graphicsFamailyIdx__,
                                              pCtxt->graphicsFamailyIdx__,
                                              image);

      image->currentLayout__ = readLayout;
      image->currentAccessMask__ = readAccessMask;
      image->currentPipeline__ = readPipeline;
      pCtxt->compiledPass.push_back(barrierPass);
      spdlog::trace("barrier pass insert");
    }
  }


  void VkGraphCompiler::writeSync(VkFrameAttachment* image)
  {
    if ((image->currentLayout__ != image->writeLayout__) ||
      (image->currentAccessMask__ != image->writeAccessMask__) ||
      (image->currentPipeline__ != image->writePipeline__) ||
      image->lastWriter__ != nullptr) //who write-> sync
    {
      VkPass barrierPass;
      barrierPass.passType = VkRenderPassType::BARRIER_PASS;
      barrierPass.execute = buildImageBarrier(image->currentAccessMask__,
                                              image->writeAccessMask__,
                                              image->currentPipeline__,
                                              image->writePipeline__,
                                              image->currentLayout__,
                                              image->writeLayout__,
                                              pCtxt->graphicsFamailyIdx__,
                                              pCtxt->graphicsFamailyIdx__,
                                              image);
      image->currentLayout__ = image->writeLayout__;
      image->currentAccessMask__ = image->writeAccessMask__;
      image->currentPipeline__ = image->writePipeline__;
      pCtxt->compiledPass.push_back(barrierPass);
    }
  }

  void VkGraphCompiler::writeSync(VkHostBuffer* fBuffer)
  {
  }

  void VkGraphCompiler::readSync(VkHostBuffer* fBuffer)
  {
  }

  void VkGraphCompiler::insertResolve(VkFrameAttachment* image)
  {
  }


  std::function<void(VkCommandBuffer cmd)> VkGraphCompiler::buildBufferBarrier(VkAccessFlags srcAccessMask,
                                                                               VkAccessFlags dstAccessMask,
                                                                               VkPipelineStageFlags srcStageMask,
                                                                               VkPipelineStageFlags dstStageMask,
                                                                               VkHostBuffer& frameBuffer,
                                                                               uint32_t srcQFamily,
                                                                               uint32_t dstQFamily,
                                                                               VkHostBuffer* buffer)
  {
    std::function<void(VkCommandBuffer cmd)> lambda;
    lambda = [&buffer,
        srcAccessMask,
        dstAccessMask,
        srcStageMask,
        dstStageMask,
        srcQFamily,
        dstQFamily](VkCommandBuffer cmd)
      {
        spdlog::debug(" resource dst StageMast {}", static_cast<uint32_t>(dstStageMask));
        spdlog::debug(" resource dst AccessMask {}", static_cast<uint32_t>(dstAccessMask));
        VkDeviceSize offsets = 0;
        VkBufferMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
        barrier.srcAccessMask = srcAccessMask;
        barrier.dstAccessMask = dstAccessMask;
        barrier.srcQueueFamilyIndex = srcQFamily;
        barrier.dstQueueFamilyIndex = dstQFamily;
        barrier.buffer = buffer->bufferh_;
        barrier.size = buffer->size_;
        barrier.offset = offsets;
        vkCmdPipelineBarrier(
                             cmd,
                             srcStageMask,
                             dstStageMask,
                             0,
                             0,
                             nullptr,
                             1,
                             &barrier,
                             0,
                             nullptr
                            );
      };
    return lambda;
  }

  std::function<void(VkCommandBuffer cmd)> VkGraphCompiler::buildImageBarrier(VkAccessFlags srcAccessMask,
                                                                              VkAccessFlags dstAccessMask,
                                                                              VkPipelineStageFlags srcStageMask,
                                                                              VkPipelineStageFlags dstStageMask,
                                                                              VkImageLayout srcImageLayout,
                                                                              VkImageLayout dstImageLayout,
                                                                              uint32_t srcQFamily,
                                                                              uint32_t dstQFamily,
                                                                              gpu::VkFrameAttachment* srcImage)
  {
    std::function<void(VkCommandBuffer cmd)> lambda;

    lambda = [srcImage,
        srcAccessMask,
        dstAccessMask,
        srcImageLayout,
        dstImageLayout,
        srcStageMask,
        dstStageMask,
        srcQFamily,
        dstQFamily](VkCommandBuffer cmd)
      {
        spdlog::debug(" resource usage {}", static_cast<uint32_t>(dstImageLayout));
        spdlog::debug(" resource usage {}", static_cast<uint32_t>(dstStageMask));
        spdlog::debug(" resource usage {}", static_cast<uint32_t>(dstImageLayout));
        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = srcImageLayout;
        barrier.newLayout = dstImageLayout;
        barrier.srcQueueFamilyIndex = srcQFamily;
        barrier.dstQueueFamilyIndex = dstQFamily;
        barrier.image = srcImage->imageh__;
        barrier.subresourceRange.aspectMask = srcImage->aspectMask__;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = srcImage->levelCount__;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.layerCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.srcAccessMask = srcAccessMask;
        barrier.dstAccessMask = dstAccessMask;
        vkCmdPipelineBarrier(
                             cmd,
                             srcStageMask,
                             dstStageMask,
                             0,
                             0,
                             nullptr,
                             0,
                             nullptr,
                             1,
                             &barrier
                            );
      };
    return lambda;
  }

  std::function<void(VkCommandBuffer cmd)> VkGraphCompiler::buildBufferCopyToImage(gpu::VkHostBuffer* srcBuffer,
    gpu::VkFrameAttachment* dstImage)
  {
    std::function<void(VkCommandBuffer cmd)> lambda;
    lambda = [srcBuffer,
        dstImage](VkCommandBuffer cmd)
      {
        spdlog::debug(" resource usage {}", static_cast<uint32_t>(dstImage->usage_));

        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = dstImage->aspectMask__;
        region.imageSubresource.mipLevel = dstImage->mipLevels__;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = dstImage->levelCount__;
        region.imageOffset = {0, 0, 0};
        region.imageExtent = {
          dstImage->width__,
          dstImage->height__,
          1
        };
        spdlog::debug("call to command buffer to trenslate to texture");
        vkCmdCopyBufferToImage(
                               cmd,
                               srcBuffer->bufferh_,
                               dstImage->imageh__,
                               dstImage->writeLayout__,
                               1,
                               &region
                              );
      };
    return lambda;
  }

  std::function<void(VkCommandBuffer cmd)> VkGraphCompiler::buildBufferCopyToBuffer(gpu::VkHostBuffer* srcBuffer,
    gpu::VkHostBuffer* dstBuffer)
  {
    std::function<void(VkCommandBuffer cmd)> lambda;
    lambda = [&srcBuffer,
        &dstBuffer](VkCommandBuffer cmd)
      {
        spdlog::debug(" resource usage {}", static_cast<uint32_t>(dstBuffer->usage_));
        VkDeviceSize offsets = 0;
        VkBufferCopy bufferCopy{};
        bufferCopy.srcOffset = 0;
        bufferCopy.dstOffset = 0;
        bufferCopy.size = dstBuffer->size_;
        vkCmdCopyBuffer(cmd,
                        srcBuffer->bufferh_,
                        dstBuffer->bufferh_,
                        1,
                        &bufferCopy);
      };
    return lambda;
  }
}

//write->reader dependency
//for (gpu::VkPass* pass : passes)
//{
//  for (auto& write__ : pass->write__)
//  {
//    maskingTimeline(write__);
//    for (auto& reader__ : write__->reader__)
//    {
//      if (pass != reader__)
//      {
//        //W->R
//        reader__->dependency__.insert(pass);
//        pass->dependent__.insert(reader__);
//      }
//    }
//    //W->W
//    if (write__->lastWriter__ != nullptr &&
//      write__->lastWriter__ != pass)
//    {
//      pass->dependency__.insert(write__->lastWriter__);
//      write__->lastWriter__->dependent__.insert(pass);
//    }
//    write__->lastWriter__ = pass;
//  }
//  for (auto read__ : pass->read__)
//  {
//    //pass->link(read__)
//    if (!read__->allocated__ ||
//      read__->dirty__)
//    {
//      allocate(read__);
//    }
//  }
//}
