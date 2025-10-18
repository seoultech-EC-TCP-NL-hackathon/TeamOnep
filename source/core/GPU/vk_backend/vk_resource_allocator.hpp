//
// Created by ljh on 25. 10. 10..
//

#ifndef MYPROJECT_VK_RESOURCE_ALLOCATOR_HPP
#define MYPROJECT_VK_RESOURCE_ALLOCATOR_HPP
#include <cstring>
#include <memory>
#include <unordered_map>
#include "vk_sampler_builder.hpp"
#include "vk_memory_allocator.hpp"

namespace gpu
{
  class VkContext;

  class VkResourceAllocator
  {
    friend class VkDiscardPool;

    public:
    VkResourceAllocator(VkContext* pCtxt);
    ~VkResourceAllocator();
    void buildMeshNode(VkMeshBuffer* buffer);
    void buildImageNode(VkImageNode* image);
    void uploadBufferTransferPass(VkBuffer src,
                                  VkBuffer dst,
                                  VkDeviceSize srcOffset,
                                  VkDeviceSize dstOffset,
                                  VkDeviceSize size);

    void uploadCopyPass(VkBuffer src,
                             VkBuffer dst,
                             VkDeviceSize srcOffset,
                             VkDeviceSize dstOffset,
                             VkDeviceSize size);

    void buildImageCopyPass(VkBuffer buffer,
                            VkTextureNode* texture);

    VkBuffer buildBufferHandle(VkDeviceSize size,
                           VkBufferUsageFlags usage);

    void buildKtxTexture(gpu::VkTextureNode* texture);
    void buildTexture(gpu::VkTextureNode* texture);

    VkBufferNode getStagingBuffer(void* data,
                                  VkDeviceSize size);
    VkAllocation mBindBuffer(VkBuffer buffer,
                             VkMemoryPropertyFlags desiredFlag);
    VkAllocation mBindImage(VkImage image,
                            VkMemoryPropertyFlags desiredFlag);
    void buildImageBarrierPass(VkImage img,
                               VkPipelineStageFlagBits src,
                               VkPipelineStageFlags dst);

    void allocateSampler();
    void allocateDescriptorSet();
    void allocateDescriptorSetLayout();
    void allocatePipeline();
    void hostUpdate(VkBufferNode* buffer__);
    void hostUpdate(VkTextureNode* texture);

    private:
    VkContext* pCtxt;
    VkDeviceSize alignUp(VkDeviceSize v, VkDeviceSize a) const;
    VkPhysicalDeviceProperties physicalDeviceProperties;
    uint32_t textureBindingSlot_;
    gpu::VkSamplerBuilder samplerBuilder_;
    //std::unique_ptr<GPU::VkShaderPool> fragShaderPool_;
    //std::unique_ptr<GPU::VkShaderPool> vertexShaderPool_;
    //std::unique_ptr<GPU::VkPipelinePool> pipelinePool_;
    //ptr binding std::unique_ptr<VkDescriptoraAlocator> pDescriptoraAllocator_;
    //std::unique
  };
}

#endif //MYPROJECT_VK_RESOURCE_ALLOCATOR_HPP
