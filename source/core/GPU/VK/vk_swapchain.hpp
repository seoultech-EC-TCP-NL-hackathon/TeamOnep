#ifndef VK_SWAPCHAIN_HPP
#define VK_SWAPCHAIN_HPP

#include "vk_common.hpp"
#include "vk_memory_allocator.hpp"

namespace gpu
{
  class VkContext;
  class VkSwapchainContext
  {
    public:
    VkSwapchainContext(VkContext* context);
    ~VkSwapchainContext();
    VkDevice device;
    VkFormat imgFormat__;
    VkSwapchainKHR swapchain__;
    std::vector<VkImage> img__;
    std::vector<VkImage> gBuffer1__;
    std::vector<VkImage> gBuffer2__;
    std::vector<VkImage> gBuffer3__;
    std::vector<VkImage> gBuffer4__;
    std::vector<VkImage> depthBuffer__;
    std::vector<VkImage> shadowBuffer__;
    std::vector<VkImage> lightningBuffer;
    std::vector<VkImageView> imgView__;
    std::vector<VkImageView> depthView__;
    std::vector<VkImageView> shadowView__;
    std::vector<VkImageView> gBufferView1__;
    std::vector<VkImageView> gBufferView2__;
    std::vector<VkImageView> gBufferView3__;
    std::vector<VkImageView> gBufferView4__;
    std::vector<VkImageView> lightningView__;
    VkBool32 broked__ = true;
    VkExtent2D extent__;
    private:
    VkPresentModeKHR choosePresentMode(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
    VkSurfaceFormatKHR chooseSurfaceFormat(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
    VkExtent2D chooseExtent(const VkSurfaceCapabilitiesKHR& capabilities, VkExtent2D desireExtent__);
  };
}


#endif
