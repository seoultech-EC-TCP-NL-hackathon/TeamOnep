#ifndef MYPROJECT_VK_DESCRIPTOR_ALLOCATOR_HPP
#define MYPROJECT_VK_DESCRIPTOR_ALLOCATOR_HPP
#include <unordered_set>
#include "vk_resource.hpp"
#include "vk_texture.hpp"
#include "vk_sampler_builder.hpp"

namespace gpu
{
  class VkContext;

  class VkDescriptorAllocator
  {
    public:
    VkDescriptorAllocator(VkContext* pCtxt);
    VkDescriptorSet allocate(uint32_t currentFrame);
    ~VkDescriptorAllocator() = default;
    void buildDefaultLayout();
    VkDescriptorSet allocateSet(VkDescriptorSetLayout layout ,
                                uint32_t count = 1);
    void writeUbo(VkBuffer srcBuffer,
                  VkDeviceSize srcSize,
                  VkDescriptorSet dstSet,
                  uint32_t dstBindingIndex,
                  uint32_t dstBindingArrayIndex,
                  uint32_t dstCount);
    void uploadBindlessTextureSet(VkTexture* texture);


    void update();
    std::vector<VkDescriptorSet> descriptorSets;
    VkDescriptorSetLayout defaultLayout;
    private:
    VkContext* pCtxt;
    uint32_t currentBindlessIndex_ = 0;
    VkDescriptorPool pool;
    VkDescriptorSet baseSet;
    gpu::VkSamplerBuilder samplerBuilder_;
    std::unordered_map<VkDescriptorSetLayout, VkDescriptorSet> descriptorSets_;
    std::vector<VkWriteDescriptorSet> writedSets_;
    std::vector<VkDescriptorBufferInfo> bufferInfos_;
    std::vector<VkDescriptorImageInfo> imageInfos_;
    //std::unique_ptr<VkDescriptorUploader> pUploader_;
  };
}


#endif //MYPROJECT_VK_DESCRIPTOR_ALLOCATOR_HPP
