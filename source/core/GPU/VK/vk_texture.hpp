//
// Created by dlwog on 25. 10. 23..
//

#ifndef MYPROJECT_VK_TEXTURE_HPP
#define MYPROJECT_VK_TEXTURE_HPP
#include "vk_resource.hpp"

namespace gpu
{
  class VkFrameAttachment : public VkResource
  {
    public:
    VkFormat format__ = VK_FORMAT_R8G8B8A8_UNORM;
    VkImageType imageType__ = VK_IMAGE_TYPE_2D;
    uint32_t height__ = 0;
    uint32_t width__ = 0;
    uint32_t mipLevels__ = 1;
    uint32_t levelCount__ = 1;
    VkImageAspectFlags aspectMask__;
    VkDescriptorSet descriptorSet__;
    VkImage imageh__ = VK_NULL_HANDLE;
    VkImageView imageView__;
    VkAllocation allocation__;
    VkSampler sampler = VK_NULL_HANDLE;
    VkImageViewType imgViewType = VK_IMAGE_VIEW_TYPE_2D;
    VkFormat imgFormat = VK_FORMAT_R8G8B8A8_SRGB;
    VkImageLayout imgLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    uint32_t depth = 0;
    uint32_t mipLevels = 0;
    uint32_t arrayLevels = 0;
    VkDeviceSize imageOffset = 0;
    VkDescriptorSet set = VK_NULL_HANDLE;
    VkDescriptorSetLayout layout = VK_NULL_HANDLE;
    uint32_t descriptorArrayIndex = 0;
    uint32_t bindigIndex = 0;
    VkBool32 writen__ = false;
    bool ktx = false;
    private:
    friend class VkGraph;
    friend class VkGraphCompiler;
    friend class VkGraphBuilder;
    friend class VkResourceAllocator;
    friend class VkScheduler;
    friend class VkDiscardPool;
    void* data__;
    VkBool32 needChangeSampler__ = false;
    std::string name;
    VkImageLayout currentLayout__ = VK_IMAGE_LAYOUT_UNDEFINED;
    VkImageLayout writeLayout__ = VK_IMAGE_LAYOUT_UNDEFINED;
    VkImageUsageFlags usage__;
  };
  using VkTexture = VkFrameAttachment;
}

#endif //MYPROJECT_VK_TEXTURE_HPP
