//
// Created by dlwog on 25. 10. 17..
//

#include "swapchainAttachment.hpp"
#include "unique.hpp"

gpu::NodeId SwapchainAttachment::buildDepthAttachment()
{
  mns::uptr<gpu::VkImageNode> depth = mns::mUptr<gpu::VkImageNode>();
  depth->type_ = gpu::ResourceType::IMAGE;
  depth->usage_ = gpu::ResourceUsage::DEPTH_STENCIL_ATTACHMENT;
  depth->aspectMask__ = VK_IMAGE_ASPECT_DEPTH_BIT;
  depth->format__ = VK_FORMAT_D32_SFLOAT;
  depth->height__ = gpu::ctx__.pSwapChainContext->extent__.height;
  depth->width__ = gpu::ctx__.pSwapChainContext->extent__.width;
  depth->mSpace_ = gpu::MemorySpace::DEVICE_LOCAL;
  depth->lifetime = gpu::VkResourceLifetime::FRAME;
  depth->mipLevels__ = 1;
  depth->levelCount__ = 1;
  return gpu::ctx__.pGraphBuilder->buildImageHandle(depth);
}
