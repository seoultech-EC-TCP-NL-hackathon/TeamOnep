#ifndef MYPROJECT_ENGINE_VK_DEVICE_HPP
#define MYPROJECT_ENGINE_VK_DEVICE_HPP

#ifdef NDEBUG
const bool ENABLE_VALIDATION_LAYERS = false;
#else
const bool ENABLE_VALIDATION_LAYERS = true;
#endif
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <cstdint>
#include <memory>
#include <optional>

#include "vk_descriptor_allocator.hpp"
#include "vk_resource_allocator.hpp"
#include "vk_memory_allocator.hpp"
#include "vk_discard_pool.hpp"

#include "vk_graph_builder.hpp"
#include "vk_node.hpp"
#include "vk_swapchain.hpp"

namespace gpu{
  class VkPass;

  class VkContext{
  public:
    void loadContext();
    GLFWwindow *windowh__;
    VkInstance instanceh__;
    VkSurfaceKHR surfaceh__;
    VkPhysicalDevice physicalDeviceh__;
    VkDevice deviceh__;
    VkQueue presentQh__;
    VkQueue graphicsQh__;
    uint32_t graphicsFamailyIdx__;
    uint32_t presentFamilyIdx__;
    VkExtent2D initExtent__{2000, 1200};

    VkBool32 dirty_              = VK_FALSE;
    VkBool32 useDynamicRendering = VK_TRUE;

    std::vector<gpu::VkPass *> uploadedPass;
    std::vector<gpu::VkPass> compiledPass;

    std::unordered_map<VkPassId, VkPass *> passHash_;
    std::unordered_map<VkNodeId, VkNode *> nodeHash_;


    std::unique_ptr<gpu::VkGraphBuilder> pGraphBuilder;
    std::unique_ptr<gpu::VkMemoryAllocator> pMemoryAllocator;
    std::unique_ptr<gpu::VkSwapchainContext> pSwapChainContext;
    std::unique_ptr<gpu::VkResourceAllocator> pResourceAllocator;
    //   std::unique_ptr<GPU::VkScheduler> pScheduler;
    std::unique_ptr<gpu::VkDiscardPool> pDiscardPool;
    std::unique_ptr<gpu::VkDescriptorAllocator> pDescriptorAllocator;
    //std::unique_ptr<GPU::VkDescriptorLayoutBuilder> pLayoutBuilder_;

    struct{
      std::vector<VkMeshBuffer *> batches;
    } batch;

    struct{
      uint32_t maxInflight__           = 3;
      uint32_t currentSwapchainIndex__ = 0;
      uint32_t currentFrame__          = 0;
    } renderingContext;

  private:
    void loadDeviceContext();
    void loadImGuiGPUContext();
   // ~VkContext();
  };
}

#endif //MYPROJECT_ENGINE_VK_DEVICE_HPP