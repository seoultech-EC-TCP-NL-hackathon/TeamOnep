//
// Created by ljh on 25. 10. 10..
//

#ifndef MYPROJECT_VK_DESCRIPTOR_ALLOCATOR_HPP
#define MYPROJECT_VK_DESCRIPTOR_ALLOCATOR_HPP

#include <unordered_set>
#include "vk_node.hpp"
//#include "vk_descriptor_uploader.hpp"
//#include "vk_descriptor_layout_builder.hpp"

namespace gpu
{
  class VkContext;

  class VkDescriptorAllocator
  {
    public:
    VkDescriptorAllocator(VkContext* pCtxt);
    VkDescriptorSet allocateLayout(VkDescriptorSetLayout layout, uint32_t count);

    void writeUbo(VkBuffer srcBuffer,
                  VkDeviceSize srcSize,
                  VkDescriptorSet dstSet,
                  uint32_t dstBindingIndex,
                  uint32_t dstBindingArrayIndex,
                  uint32_t dstCount);
    void uploadBindlessTextureSet(VkTextureNode* texture);


    void update();
    ~VkDescriptorAllocator() = default;
    void allocateDescriptorSet(VkNode* node);

    private:
    VkContext* pCtxt;
    uint32_t currentBindlessIndex_ = 0;
    VkDescriptorPool pool;
    std::unordered_map<VkDescriptorSetLayout, VkDescriptorSet> descriptorSets_;
    std::vector<VkWriteDescriptorSet> writedSets_;
    std::vector<VkDescriptorBufferInfo> bufferInfos_;
    std::vector<VkDescriptorImageInfo> imageInfos_;
    //std::unique_ptr<VkDescriptorUploader> pUploader_;
  };
}


#endif //MYPROJECT_VK_DESCRIPTOR_ALLOCATOR_HPP
