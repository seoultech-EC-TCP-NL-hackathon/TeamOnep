#include <memory>
#include "spdlog/spdlog.h"
#include "vk_resource_allocator.hpp"
#include "stb_image/stb_image.h"
#include "vk_context.hpp"
#include "vk_swapchain.hpp"
#include "ktx.h"

gpu::VkResourceAllocator::VkResourceAllocator(VkContext* pCtxt) :
  pCtxt(pCtxt),
  samplerBuilder_(pCtxt)

{
}

gpu::VkResourceAllocator::~VkResourceAllocator() = default;
//그냥 바로 올리면 되지않음?
//tranfer인데 어차피 기다릴게 없는데
//compile하지 않고 바로 올리고 buffer copy해도 되잔아
//image, texture, mesh 모두 다 그렇잔아
void gpu::VkResourceAllocator::buildMeshNode(VkMeshNode* buffer)
{
  buildBufferHandle(buffer->vSize__,
                    VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                    &buffer->vertexBuffer__);
  VkBufferNode vStaging = getStagingBuffer(buffer->vData__,
                                           buffer->vSize__);
  buildBufferCopyPass(vStaging.bufferh_,
                      buffer->vertexBuffer__,
                      0,
                      0,
                      buffer->vSize__);
  buildBufferHandle(buffer->iSize__,
                    VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                    &buffer->indexBuffer__);
  VkBufferNode iStaging = getStagingBuffer(buffer->iData__,
                                           buffer->iSize__);

  buildBufferCopyPass(iStaging.bufferh_,
                      buffer->indexBuffer__,
                      0,
                      0,
                      buffer->iSize__);


  buffer->hostUpdate__ = true;
  buffer->allocated__ = true;
}

gpu::VkBufferNode gpu::VkResourceAllocator::getStagingBuffer(void* data,
                                                             VkDeviceSize size)
{
  VkBufferNode stagingBuffer{};
  stagingBuffer.data_ = data;
  buildBufferHandle(size,
                    VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                    &stagingBuffer.bufferh_);
  mBindBuffer(&stagingBuffer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
  hostUpdate(&stagingBuffer);
  return stagingBuffer;
}


void gpu::VkResourceAllocator::buildImageNode(VkImageNode* image)
{
  VkImageCreateInfo imageInfo{};
  imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  imageInfo.imageType = VK_IMAGE_TYPE_2D;
  imageInfo.extent.width = image->width__;
  imageInfo.extent.height = image->height__;
  imageInfo.extent.depth = 1;
  imageInfo.mipLevels = 1;
  imageInfo.arrayLayers = 1;

  imageInfo.format = image->format__;
  imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
  imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  imageInfo.usage = image->usage__;
  imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
  imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateImage(pCtxt->deviceh__,
                    &imageInfo,
                    nullptr,
                    &image->imageh__) != VK_SUCCESS)
  {
    throw std::runtime_error("failed to create depth image!");
  }
  VkImageViewCreateInfo view{};
  view.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  view.image = image->imageh__;
  view.viewType = VK_IMAGE_VIEW_TYPE_2D;
  view.format = image->format__;

  view.subresourceRange.aspectMask = image->aspectMask__;
  view.subresourceRange.baseMipLevel = 0;
  view.subresourceRange.levelCount = 1;
  view.subresourceRange.baseArrayLayer = 0;
  view.subresourceRange.layerCount = 1;
  if (vkCreateImageView(pCtxt->deviceh__,
                        &view,
                        nullptr,
                        &image->imageView__) != VK_SUCCESS)
  {
    throw std::runtime_error("fail to make depth View");
  }
}

void gpu::VkResourceAllocator::uploadBufferTransferPass(VkBuffer src,
                                                        VkBuffer dst,
                                                        VkDeviceSize srcOffset,
                                                        VkDeviceSize dstOffset,
                                                        VkDeviceSize size)
{
  gpu::VkPass copyPass{
    .passType = RenderPassType::COPY_PASS,
    .read__ = {},
    .write__ = {},
    .execute =
    [&src,
      &dst,
      &srcOffset,
      &dstOffset,
      &size]
  (VkCommandBuffer cmd)
    {
      VkBufferCopy region{};
      region.srcOffset = srcOffset;
      region.dstOffset = dstOffset;
      region.size = size;
      vkCmdCopyBuffer(cmd, src, dst, 1, &region);
    }
  };
  copyPass.transitionPass = true;
  pCtxt->compiledPass.push_back(copyPass);
}

void gpu::VkResourceAllocator::buildBufferCopyPass(VkBuffer src,
                                                   VkBuffer dst,
                                                   VkDeviceSize srcOffset,
                                                   VkDeviceSize dstOffset,
                                                   VkDeviceSize size)
{
  VkPass copyPass{
    .passType = RenderPassType::COPY_PASS,
    .read__ = {},
    .write__ = {},
    .execute =
    [&src,
      &dst,
      &srcOffset,
      &dstOffset,
      &size]
  (VkCommandBuffer cmd)
    {
      VkBufferCopy region{};
      region.srcOffset = srcOffset;
      region.dstOffset = dstOffset;
      region.size = size;
      vkCmdCopyBuffer(cmd, src, dst, 1, &region);
    }
  };
  copyPass.transitionPass = true;
  pCtxt->compiledPass.push_back(copyPass);
}

void gpu::VkResourceAllocator::buildImageCopyPass(VkBuffer buffer,
                                                  VkTextureNode* texture)
{
  VkPass copyPass{
    .passType = RenderPassType::COPY_PASS,
    .read__ = {},
    .write__ = {},
    .execute =
    [&buffer, texture](VkCommandBuffer cmd)
    {
      VkBufferImageCopy region{};
      region.bufferOffset = 0;
      region.bufferRowLength = 0;
      region.bufferImageHeight = 0;
      region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      region.imageSubresource.mipLevel = 0;
      region.imageSubresource.baseArrayLayer = 0;
      region.imageSubresource.layerCount = 1;
      region.imageOffset = {0, 0, 0};
      region.imageExtent = {
        texture->width__,
        texture->height__,
        1
      };
      spdlog::info("call to command buffer to trenslate to texture");
      vkCmdCopyBufferToImage(
                             cmd,
                             buffer,
                             texture->imageh__,
                             VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                             1,
                             &region
                            );
    }
  };
  copyPass.transitionPass = true;
  pCtxt->compiledPass.push_back(copyPass);
}

void gpu::VkResourceAllocator::buildImageBarrierPass(VkImage img,
                                                     VkPipelineStageFlagBits src,
                                                     VkPipelineStageFlags dst)
{
  VkPass BarrierPass{
    .passType = RenderPassType::BARRIER_PASS,
    .read__ = {},
    .write__ = {},
    .execute =
    [&img,
      &src,
      &dst]
  (VkCommandBuffer cmd)
    {
      VkImageMemoryBarrier barrier{};
      barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
      barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
      barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
      barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.image = img;
      barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      barrier.subresourceRange.baseMipLevel = 0;
      barrier.subresourceRange.levelCount = 1;
      barrier.subresourceRange.baseArrayLayer = 0;
      barrier.subresourceRange.layerCount = 1;
      barrier.srcAccessMask = 0;
      barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
      vkCmdPipelineBarrier(cmd,
                           src,
                           dst,
                           0,
                           0,
                           nullptr,
                           0,
                           nullptr,
                           1,
                           &barrier
                          );
    }
  };
  BarrierPass.transitionPass = true;
  pCtxt->compiledPass.push_back(BarrierPass);
}

void gpu::VkResourceAllocator::mBindImage(VkImageNode* image,
                                          VkMemoryPropertyFlags desiredFlag)
{
  VkMemoryRequirements memRequirements;
  VkMemoryPropertyFlags properties = desiredFlag;

  vkGetImageMemoryRequirements(pCtxt->deviceh__,
                               image->imageh__,
                               &memRequirements);
  VkAllocation allocation = pCtxt->pMemoryAllocator->allocate(memRequirements,
                                                              properties);

  mAlloc_[image->nodeId_] = allocation;
  if (vkBindImageMemory(pCtxt->deviceh__,
                        image->imageh__,
                        allocation.memory__,
                        allocation.offset__) != VK_SUCCESS)
  {
    throw std::runtime_error("fail to allocate depth memroy");
  }
}


VkDeviceSize gpu::VkResourceAllocator::alignUp(VkDeviceSize v, VkDeviceSize a) const
{
  return (v + (a - 1)) & ~(a - 1);
}

void gpu::VkResourceAllocator::mBindBuffer(VkBufferNode* buffer,
                                           VkMemoryPropertyFlags desiredFlag)
{
  VkMemoryRequirements memRequirements;
  vkGetBufferMemoryRequirements(pCtxt->deviceh__, buffer->bufferh_, &memRequirements);
  VkMemoryPropertyFlags properties = desiredFlag;

  VkAllocation allocation = pCtxt->pMemoryAllocator->allocate(memRequirements, properties);

  if (vkBindBufferMemory(pCtxt->deviceh__,
                         buffer->bufferh_,
                         allocation.memory__,
                         allocation.offset__) != VK_SUCCESS)
  {
    spdlog::info("falil to bind buffer to memory");
  }
}

void gpu::VkResourceAllocator::hostUpdate(VkBufferNode* buffer__)
{
  void* data = nullptr;
  VkAllocation* alloc = &mAlloc_[buffer__->nodeId_];
  vkMapMemory(pCtxt->deviceh__,
              alloc->memory__,
              alloc->offset__,
              alloc->size,
              0,
              &data);
  std::memcpy(data,
              buffer__->data_,
              (size_t)buffer__->size_);

  vkUnmapMemory(pCtxt->deviceh__,
                alloc->memory__);
}


void gpu::VkResourceAllocator::buildBufferHandle(VkDeviceSize size,
                                                 VkBufferUsageFlags usage,
                                                 VkBuffer* pBuffer)
{
  VkBufferCreateInfo bufferInfo{
    .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
    .size = size,
    .usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
    .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
  };
  if (vkCreateBuffer(pCtxt->deviceh__,
                     &bufferInfo,
                     nullptr,
                     pBuffer) != VK_SUCCESS)
  {
    throw std::runtime_error("failed to create buffer!");
  }
}

//void gpu::VkResourceAllocator::buildKtxTexture(gpu::VkTextureNode* texture)
//{
//  std::vector<VkBufferImageCopy> bufferCopyRegions;
//  for (uint32_t i = 0; i < texture.mipLevels; i++)
//  {
//    // Calculate offset into staging buffer for the current mip level
//    ktx_size_t offset;
//    KTX_error_code ret = ktxTexture_GetImageOffset(ktxTexture, i, 0, 0, &offset);
//    assert(ret == KTX_SUCCESS);
//    // Setup a buffer image copy structure for the current mip level
//    VkBufferImageCopy bufferCopyRegion = {};
//    bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
//    bufferCopyRegion.imageSubresource.mipLevel = i;
//    bufferCopyRegion.imageSubresource.baseArrayLayer = 0;
//    bufferCopyRegion.imageSubresource.layerCount = 1;
//    bufferCopyRegion.imageExtent.width = ktxTexture->baseWidth >> i;
//    bufferCopyRegion.imageExtent.height = ktxTexture->baseHeight >> i;
//    bufferCopyRegion.imageExtent.depth = 1;
//    bufferCopyRegion.bufferOffset = offset;
//    bufferCopyRegions.push_back(bufferCopyRegion);
//  }
//}
//
void gpu::VkResourceAllocator::buildTexture(gpu::VkTextureNode* texture)
{
  VkImageCreateInfo imageInfo{};
  imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  imageInfo.imageType = VK_IMAGE_TYPE_2D;
  imageInfo.flags = 0;
  imageInfo.extent.width = texture->width__;
  imageInfo.extent.height = texture->height__;
  imageInfo.extent.depth = 1;
  imageInfo.mipLevels = 1;
  imageInfo.arrayLayers = 1;
  imageInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
  imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
  imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
  imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
  imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  imageInfo.pNext = nullptr;
  if (vkCreateImage(pCtxt->deviceh__, &imageInfo, nullptr, &texture->img__) != VK_SUCCESS)
  {
    spdlog::info("error ");
    std::runtime_error("fail to make texture Image buffer");
  }
  mBindImage(texture,
             VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  VkImageViewCreateInfo viewInfo{};
  viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  viewInfo.image = texture->img__;
  viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  viewInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
  viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
  viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
  viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
  viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
  viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  viewInfo.subresourceRange.baseMipLevel = 0;
  viewInfo.subresourceRange.levelCount = 1;
  viewInfo.subresourceRange.baseArrayLayer = 0;
  viewInfo.subresourceRange.layerCount = 1;
  if (vkCreateImageView(pCtxt->deviceh__,
                        &viewInfo,
                        nullptr,
                        &texture->imageView__) != VK_SUCCESS)
  {
    throw std::runtime_error("error to make texture view");
  }
  VkDeviceSize size = texture->width__ * texture->height__ * 4;
  VkBufferNode staging = getStagingBuffer(texture->data__, size);
  //buildImageBarrierPass(texture->imageh__ )
  //buildImageCopyPass.
  //buildImageBarrierPass
}
