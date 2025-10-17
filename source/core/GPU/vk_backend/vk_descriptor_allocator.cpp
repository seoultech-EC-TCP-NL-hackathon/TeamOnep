//
// Created by ljh on 25. 10. 10..
//

#include "vk_descriptor_allocator.hpp"
#include "vk_context.hpp"
#include "vk_node.hpp"

gpu::VkDescriptorAllocator::VkDescriptorAllocator(VkContext* pCtxt) :
  pCtxt(pCtxt)
{
  VkDescriptorPoolSize PoolSize[] = {
    {VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
    {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
    {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
    {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
    {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
    {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
    {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
    {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
    {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
    {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
    {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}
  };
  VkDescriptorPoolCreateInfo poolInfo = {};
  poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
  poolInfo.maxSets = 1000;
  poolInfo.poolSizeCount = 1000 * sizeof(VkDescriptorPoolSize);
  poolInfo.pPoolSizes = PoolSize;
  //if (vkCreateDescriptorPool(pCtxt->deviceh__,
  //                           &poolInfo,
  //                           nullptr,
  //                           &pool) != VK_SUCCESS)
  //{
  //  throw std::runtime_error("fail to create Imgui Pool");
  //}
}

VkDescriptorSet gpu::VkDescriptorAllocator::allocateLayout(VkDescriptorSetLayout layout, uint32_t count)
{
  VkDescriptorSetAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = pool;
  allocInfo.descriptorSetCount = 1;
  allocInfo.pSetLayouts = &layout;
  VkDescriptorSet set;
  if (vkAllocateDescriptorSets(pCtxt->deviceh__,
                               &allocInfo,
                               &set) != VK_SUCCESS)
    throw std::runtime_error("failed to allocate descriptor set");
  return set;
}

void gpu::VkDescriptorAllocator::writeUbo(VkBuffer srcBuffer,
                                          VkDeviceSize srcSize,
                                          VkDescriptorSet dstSet,
                                          uint32_t dstBindingIndex,
                                          uint32_t dstBindingArrayIndex,
                                          uint32_t dstCount)
{
  VkDescriptorBufferInfo bufferInfo{};
  bufferInfo.buffer = srcBuffer;
  bufferInfo.range = VK_WHOLE_SIZE;
  bufferInfo.offset = 0;

  VkWriteDescriptorSet writeDescriptorSet{};
  writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  writeDescriptorSet.dstSet = dstSet;
  writeDescriptorSet.dstBinding = dstBindingIndex;
  writeDescriptorSet.pBufferInfo = &bufferInfo;
  writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  writeDescriptorSet.descriptorCount = dstCount;
  writeDescriptorSet.dstArrayElement = dstBindingArrayIndex;
  vkUpdateDescriptorSets(pCtxt->deviceh__, 1, &writeDescriptorSet, 0, nullptr);
}

void gpu::VkDescriptorAllocator::uploadBindlessTextureSet(VkTextureNode* texture)
{
  VkDescriptorImageInfo imageInfo{};
  imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  imageInfo.imageView = texture->view__;
  imageInfo.sampler = texture->sampler;
  imageInfos_.push_back(imageInfo);

  VkWriteDescriptorSet descriptorWrite{};
  descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  descriptorWrite.dstSet = texture->descriptor;
  descriptorWrite.dstBinding = currentBindlessIndex_;
  descriptorWrite.dstArrayElement = texture->descriptorArrayIndex;
  descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  descriptorWrite.descriptorCount = 1;
  descriptorWrite.pImageInfo = &imageInfos_[imageInfos_.size() - 1];
  writedSets_.push_back(descriptorWrite);
  texture->bindigIndex = currentBindlessIndex_;
}

void gpu::VkDescriptorAllocator::update()
{
  vkUpdateDescriptorSets(pCtxt->deviceh__,
                         writedSets_.size(),
                         writedSets_.data(),
                         0,
                         nullptr);
  bufferInfos_.clear();
  imageInfos_.clear();
  writedSets_.clear();
}

//
//void gpu::VkDescriptorAllocator::free()
//{
//
//}
