
#ifndef IMAGEVIEW_HPP
#define IMAGEVIEW_HPP 
#include "common.hpp"
#include "vk_memory_allocator.hpp"
#include "vk_memory_pool.hpp"
struct ViewManagerCreateInfo{
  VkDevice                    device;
  const std::vector<VkImage> *images;
  VkFormat                    format;
  VkExtent2D                  extent;
  gpu::VkMemoryAllocator *           allocator;
};

class SwapchainViewManager{
public:
  SwapchainViewManager(const ViewManagerCreateInfo &info);
  ~SwapchainViewManager();
  const std::vector<VkImageView> &getImageViews() const;
  const std::vector<VkImageView> &getDepthViews() const;

private:
  VkDevice                 device;
  VkExtent2D               extent;
  std::vector<VkImageView> imageViews;
  std::vector<VkImageView> depthViews;
  std::vector<VkImage>     depthImage;
  std::vector<gpu::VkAllocation>  allocations;
  gpu::VkMemoryAllocator &        allocator;
  void setDepthView();

  gpu::VkAllocation memoryBind(uint32_t index);
};

#endif