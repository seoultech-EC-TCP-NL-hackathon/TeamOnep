//
// Created by ljh on 25. 10. 7..
//
#include "vk_context.hpp"
#include "vk_scheduler.hpp"
#include "vk_graph_compiler.hpp"
#include "vk_graph.hpp"

gpu::VkScheduler::VkScheduler(gpu::VkContext* context) :
  pCtxt_(context),
  imageAvailiableSemaphorePool_(context),
  renderFinishSemaphorePool_(context),
  fencePool_(context),
  commandBufferPool_(context),
  graphs_(context),
  graphCompiler_(context)
{
  //vkCmdSetPolygonModeEXT = (PFN_vkCmdSetPolygonModeEXT)
  //  vkGetDeviceProcAddr(pCtxt_->deviceh__, "vkCmdSetPolygonModeEXT");
  //if (!vkCmdSetPolygonModeEXT)
  //{
  //  throw std::runtime_error("vkCmdSetPolygonModeEXT not available!");
  //}
}

gpu::VkScheduler::~VkScheduler() = default;

void gpu::VkScheduler::runGraphicsPipeline()
{
  uint64_t fenceTime = 1000000;
  VkFence vkFence = fencePool_.fences[pCtxt_->
                                      renderingContext.currentFrame__];

  vkWaitForFences(pCtxt_->deviceh__,
                  1,
                  &vkFence,
                  VK_TRUE,
                  UINT64_MAX);

  vkResetFences(pCtxt_->deviceh__,
                1,
                &vkFence);


  VkResult result = vkAcquireNextImageKHR(pCtxt_->deviceh__,
                                          pCtxt_->pSwapChainContext->swapchain__,
                                          UINT64_MAX,
                                          (imageAvailiableSemaphorePool_.semaphores__
                                            [pCtxt_->renderingContext.currentFrame__]),
                                          VK_NULL_HANDLE,
                                          &(pCtxt_->renderingContext.currentSwapchainIndex__));
  auto swapchain = reinterpret_cast<VkImageNode*>(pCtxt_->nodeHash_[pCtxt_->pGraphBuilder->getSwapchainImage()]);
  swapchain->nodeName_ = "swapchain image";
  //swapchain->imageh__ = pCtxt_->pSwapChainContext->img__[pCtxt_->renderingContext.currentSwapchainIndex__];
  //swapchain->imageView__ = pCtxt_->pSwapChainContext->imgView__[pCtxt_->renderingContext.currentSwapchainIndex__];

  if (result == VK_ERROR_OUT_OF_DATE_KHR)
  {
    pCtxt_->pSwapChainContext->broked__ = true;
    //this->swapchain -> recreate()
    //this->recreateSwapchain__ = VK_TRUE;
    //this->passes_.clear();
  }
  else if ((result != VK_SUCCESS) && (result != VK_SUBOPTIMAL_KHR))
  {
    throw std::runtime_error("Could not acquire the next swap chain image!");
  }
  VkCommandBuffer cmd = commandBufferPool_.
    commandBuffers__[pCtxt_->renderingContext.currentFrame__];
  if (vkResetCommandBuffer(cmd, 0) != VK_SUCCESS)
  {
    throw std::runtime_error("fail to reset command buffer");
  }
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
  beginInfo.pInheritanceInfo = nullptr;

  if (vkBeginCommandBuffer(cmd, &beginInfo) != VK_SUCCESS)
  {
    throw std::runtime_error("failed to begin recording command buffer!");
  }

  if (pCtxt_->dirty_)
  {
    pCtxt_->dirty_ = false;
    graphCompiler_.compileGraph();
  }
  graphs_.execute(cmd);
  vkEndCommandBuffer(cmd);
  //spdlog::debug("begin Rendering");


  VkSubmitInfo submitInfo{};
  VkSemaphore waitSemaphores[] = {
    (imageAvailiableSemaphorePool_.semaphores__
      [pCtxt_->renderingContext.currentFrame__])
  };

  VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSemaphores;
  submitInfo.pWaitDstStageMask = waitStages;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &cmd;

  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = &renderFinishSemaphorePool_.semaphores__
    [pCtxt_->renderingContext.currentFrame__];

  if (vkQueueSubmit(this->pCtxt_->graphicsQh__,
                    1,
                    &submitInfo,
                    fencePool_.fences
                    [pCtxt_->renderingContext.currentFrame__]) != VK_SUCCESS)
  {
    throw std::runtime_error("failed to submit draw command buffer!");
  }


  VkSwapchainKHR swapchains[] = {pCtxt_->pSwapChainContext->swapchain__};
  VkPresentInfoKHR presentInfo{};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = &renderFinishSemaphorePool_.semaphores__
    [pCtxt_->renderingContext.currentFrame__];

  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapchains;
  presentInfo.pImageIndices = &(pCtxt_->renderingContext.currentSwapchainIndex__);
  if (vkQueuePresentKHR(this->pCtxt_->presentQh__, &presentInfo) != VK_SUCCESS)
  {
    throw std::runtime_error("present Queue error");
  }
  pCtxt_->renderingContext.currentFrame__ = (pCtxt_->renderingContext.currentFrame__ + 1) %
    (pCtxt_->renderingContext.maxInflight__);
  ////spdlog::debug(" present");
}
