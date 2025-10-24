#include "../GPU/context.hpp"
#include "material.hpp"
#include "renderer.hpp"
#include "cast.hpp"

IRenderer::IRenderer(RenderInitInfo info) : pResourceManager(*info.resourceManager)
{
  vkCmdSetPolygonModeEXT = (PFN_vkCmdSetPolygonModeEXT)vkGetDeviceProcAddr(gpu::ctx__->deviceh__,
                                                                           "vkCmdSetPolygonModeEXT");
  if (!vkCmdSetPolygonModeEXT)
  {
    throw std::runtime_error("vkCmdSetPolygonModeEXT not available!");
  }
  std::uintptr_t vShader = gpu::iShd__->getShader(vertPath, shaderc_vertex_shader);
  std::uintptr_t fshader = gpu::iShd__->getShader(fragPath, shaderc_fragment_shader);
  pipelineLayout_h = gpu::ctx__->pPipelinePool->createPipelineLayout(&gpu::ctx__->pDescriptorAllocator->defaultLayout,
                                                                     1);
  gpu::VkPipelineProgram program{};
  VkFormat swapchainfFormat = gpu::ctx__->pSwapChainContext->imgFormat__;
  program.renderingType = gpu::RenderingAttachmentType::DEPTH;
  program.pipelineLayout = pipelineLayout_h;
  program.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  program.vertShaderModule = cast<VkShaderModule>(vShader);
  program.fragShaderModule = cast<VkShaderModule>(fshader);
  program.vertexType = gpu::VertexType::ALL;
  pipeline_h = gpu::ctx__->pPipelinePool->createPipeline(program);

  std::uintptr_t vQuad = gpu::iShd__->getShader(VertBackPath, shaderc_vertex_shader);
  std::uintptr_t fQuad = gpu::iShd__->getShader(fragBackPath, shaderc_fragment_shader);
  program.vertShaderModule = cast<VkShaderModule>(vQuad);
  program.fragShaderModule = cast<VkShaderModule>(fQuad);
  program.vertexType = gpu::VertexType::BACKGROUND;
  program.renderingType = gpu::RenderingAttachmentType::COLOR;
  backgroundPipeline_ = gpu::ctx__->pPipelinePool->createPipeline(program);
  drawBackground = VK_TRUE;
  for (uint32_t i = 0; i < gpu::ctx__->renderingContext.maxInflight__; i++)
  {
    depthAttachmentHandle_.push_back(gpu::VkGraphBuilder::buildDepthAttachment());
  }
}

void IRenderer::uploadDepthPass()
{
  gpu::NodeId swapchain = gpu::VkGraphBuilder::getSwapchainImage();
  mns::uptr<gpu::RenderPass> pass = mns::mUptr<gpu::RenderPass>();
  pass->passType = gpu::RenderPassType::DEPTH_PASS;
  gpu::RenderPass* ptr = pass.get();
  ptr->dependency__ = {};
  ptr->dependent__ = {};
  ptr->linkCount = 0;
  pass->execute = [this, ptr](gpu::CommandBuffer cmd)
  {
    vkCmdBindDescriptorSets(cmd,
                             VK_PIPELINE_BIND_POINT_GRAPHICS,
                             pipelineLayout_h,
                             0,
                             1,
                             &gpu::ctx__->pDescriptorAllocator->descriptorSets
                             [gpu::ctx__->renderingContext.currentFrame__],
                             0,
                             nullptr);
    gpu::cmdBeginRendering(cmd, ptr);
    pushConstant(cmd);
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_h);
    vkCmdSetPolygonModeEXT(cmd, polygonMode);
    vkCmdSetDepthTestEnable(cmd, depthTest);
    gpu::cmdSetViewports(cmd,
                         0.0,
                         0.0,
                         viewport.width = (float)gpu::ctx__->pSwapChainContext->extent__.width,
                         viewport.height = (float)gpu::ctx__->pSwapChainContext->extent__.height
                        );
    for (auto handle : drawHandle_)
      handle->draw(cmd);
    gpu::cmdEndRendering(cmd);
  };
  depthAttachmentHandle_[gpu::ctx__->renderingContext.currentFrame__]->lastWriter__ = nullptr;
  depthAttachmentHandle_[gpu::ctx__->renderingContext.currentFrame__]->writen__ = false;
  for (auto handle : drawHandle_)
  {
    pass->read__.push_back(handle);
  }
  pass->write__.push_back(depthAttachmentHandle_[gpu::ctx__->renderingContext.currentFrame__]);
  gpu::PassId renderPass = gpu::VkGraphBuilder::addPass(pass);
}

void IRenderer::uploadQuadDraw()
{
  gpu::NodeId swapchain = gpu::ctx__->graphBuilder.getSwapchainImage();
  mns::uptr<gpu::RenderPass> pass = mns::mUptr<gpu::RenderPass>();
  pass->passType = gpu::RenderPassType::G_BUFFER_PASS;
  gpu::RenderPass* ptr = pass.get();
  ptr->dependency__ = {};
  ptr->dependent__ = {};
  ptr->linkCount = 0;
  this ->renderAttachment.DepthBuffer = 0;
  this->renderAttachment.DepthBuffer = depthAttachmentHandle_[gpu::ctx__->renderingContext.currentFrame__]->descriptorArrayIndex;;
  pass->execute = [this, ptr](gpu::CommandBuffer cmd)
  {
    gpu::cmdBeginRendering(cmd, ptr);
    pushConstant(cmd);
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, backgroundPipeline_);
    vkCmdSetPolygonModeEXT(cmd, polygonMode);
    vkCmdSetDepthTestEnable(cmd, depthTest);
    gpu::cmdSetViewports(cmd,
                         0.0,
                         0.0,
                         viewport.width = (float)gpu::ctx__->pSwapChainContext->extent__.width,
                         viewport.height = (float)gpu::ctx__->pSwapChainContext->extent__.height
                        );
    gpu::cmdDrawQuad(cmd);
    gpu::cmdEndRendering(cmd);
  };
  depthAttachmentHandle_[gpu::ctx__->renderingContext.currentFrame__]->lastWriter__ = nullptr;
  depthAttachmentHandle_[gpu::ctx__->renderingContext.currentFrame__]->writen__ = false;
  pass->read__.push_back(depthAttachmentHandle_[gpu::ctx__->renderingContext.currentFrame__]);
  gpu::PassId renderPass = gpu::ctx__->graphBuilder.addPass(pass);
  gpu::ctx__->graphBuilder.addWriteResource(renderPass, swapchain);
}

void IRenderer::uploadUiDraw()
{
  gpu::NodeId swapchain = gpu::ctx__->graphBuilder.getSwapchainImage();
  mns::uptr<gpu::RenderPass> pass = mns::mUptr<gpu::RenderPass>();
  pass->passType = gpu::RenderPassType::UI;
  gpu::RenderPass* ptr = pass.get();
  ptr->dependency__ = {};
  ptr->dependent__ = {};
  ptr->linkCount = 0;
  pass->execute = [ptr](gpu::CommandBuffer cmd)
  {
    gpu::cmdBeginRendering(cmd, ptr);
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);
    gpu::cmdEndRendering(cmd);
  };
  gpu::PassId uiPass = gpu::ctx__->graphBuilder.addPass(pass);
  gpu::ctx__->graphBuilder.addWriteResource(uiPass, swapchain);
}

void IRenderer::pushConstant(VkCommandBuffer command)
{
  vkCmdPushConstants(
                     command,
                     pipelineLayout_h,
                     VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                     0,
                     sizeof(renderAttachment),
                     &renderAttachment
                    );
  //spdlog::trace("albedo index: {} ",pResourceManager.selectedModel.constant.albedoTextureIndex) ;
  //spdlog::trace("metalic index: {} ",pResourceManager.selectedModel.constant.metalicTextureIndex) ;
  //spdlog::trace("normal index: {} ",pResourceManager.selectedModel.constant.normalTextureIndex) ;
}


void IRenderer::draw(VkCommandBuffer cmd, uint32_t currenFrame)
{
  if (drawBackground)
  {
    vkCmdSetPolygonModeEXT(cmd, polygonMode);
    vkCmdSetDepthTestEnable(cmd, depthTest);
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, backgroundPipeline_);
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)gpu::ctx__->pSwapChainContext->extent__.width;
    viewport.height = (float)gpu::ctx__->pSwapChainContext->extent__.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(cmd, 0, 1, &viewport);
    scissor.offset = {0, 0};
    scissor.extent = gpu::ctx__->pSwapChainContext->extent__;
    vkCmdSetScissor(cmd, 0, 1, &scissor);
    vkCmdDraw(cmd, 6, 1, 0, 0);
  }

  vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_h);
  vkCmdSetPolygonModeEXT(cmd, polygonMode);
  vkCmdSetDepthTestEnable(cmd, depthTest);
  switch (viewMode)
  {
    case (ViewMode::SINGLE):
    {
      pushConstant(cmd);
      viewport.x = 0.0f;
      viewport.y = 0.0f;
      viewport.width = (float)gpu::ctx__->pSwapChainContext->extent__.width;
      viewport.height = (float)gpu::ctx__->pSwapChainContext->extent__.height;
      viewport.minDepth = 0.0f;
      viewport.maxDepth = 1.0f;
      vkCmdSetViewport(cmd, 0, 1, &viewport);

      scissor.offset = {0, 0};
      scissor.extent = gpu::ctx__->pSwapChainContext->extent__;
      vkCmdSetScissor(cmd, 0, 1, &scissor);

      for (auto& mesh : pResourceManager.meshes_)
      {
        if (mesh.second == nullptr) return;
        mesh.second->bind(cmd);
        mesh.second->draw(cmd);
      }
      break;
    }
    case (ViewMode::MULTI):
    {
      auto extent = gpu::ctx__->pSwapChainContext->extent__;
      float halfWidth = extent.width / 2.0f;
      float halfHeight = extent.height / 2.0f;
      for (uint32_t i = 0; i < 4; i++)
      {
        viewport.width = halfWidth;
        viewport.height = halfHeight;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        viewport.x = (i % 2) * halfWidth;
        viewport.y = (i / 2) * halfHeight;
        vkCmdSetViewport(cmd, 0, 1, &viewport);
        pushConstant(cmd);
        scissor.offset = {static_cast<int32_t>(viewport.x), static_cast<int32_t>(viewport.y)};
        scissor.extent = {static_cast<uint32_t>(viewport.width), static_cast<uint32_t>(viewport.height)};
        vkCmdSetScissor(cmd, 0, 1, &scissor);
        for (auto& mesh : pResourceManager.meshes_)
        {
          mesh.second->bind(cmd);
          mesh.second->draw(cmd);
        }
      }
      break;
    }
    case (ViewMode::VR):
    {
      pushConstant(cmd);

      pResourceManager.updateMaincamState(currenFrame);
      viewport.x = 0.0f;
      viewport.y = 0.0f;
      viewport.width = (float)gpu::ctx__->pSwapChainContext->extent__.width / 2;
      viewport.height = (float)gpu::ctx__->pSwapChainContext->extent__.height;
      viewport.minDepth = 0.0f;
      viewport.maxDepth = 1.0f;
      vkCmdSetViewport(cmd, 0, 1, &viewport);

      scissor.offset = {0, 0};
      scissor.extent = gpu::ctx__->pSwapChainContext->extent__;
      vkCmdSetScissor(cmd, 0, 1, &scissor);

      for (auto& mesh : pResourceManager.meshes_)
      {
        if (mesh.second == nullptr) return;
        mesh.second->bind(cmd);
        mesh.second->draw(cmd);
      }
      pushConstant(cmd);

      pResourceManager.updateMaincamState(currenFrame);
      viewport.x = gpu::ctx__->pSwapChainContext->extent__.width / 2;
      viewport.y = 0.0f;
      viewport.width = (float)gpu::ctx__->pSwapChainContext->extent__.width / 2;
      viewport.height = (float)gpu::ctx__->pSwapChainContext->extent__.height;
      viewport.minDepth = 0.0f;
      viewport.maxDepth = 1.0f;
      vkCmdSetViewport(cmd, 0, 1, &viewport);

      scissor.offset = {0, 0};
      scissor.extent = gpu::ctx__->pSwapChainContext->extent__;
      vkCmdSetScissor(cmd, 0, 1, &scissor);

      for (auto& mesh : pResourceManager.meshes_)
      {
        if (mesh.second == nullptr) return;
        mesh.second->bind(cmd);
        mesh.second->draw(cmd);
      }
      break;
    }
    default:
      break;
  }
}

//
//mns::uptr<gpu::RenderPass> pass = mns::mUptr<gpu::RenderPass>();
//pass->passType                  = gpu::RenderPassType::UI;
//gpu::RenderPass *ptr            = pass.get();
//
//mns::uptr<gpu::RenderPass> draw = mns::mUptr<gpu::RenderPass>();
//auto drawPtr = draw.get();
//draw->passType                  = gpu::RenderPassType::FORWARD_PASS;
//draw->execute = [this, &drawPtr](gpu::CommandBuffer cmd)
//  {
//    gpu::RenderingInfo renderingInfo{
//    .sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR,
//    .layerCount = 1,
//    };
//    renderingInfo.colorAttachmentCount = drawPtr->passParameter__.colorAttachment__.size();
//    renderingInfo.pColorAttachments    = drawPtr->passParameter__.colorAttachment__.data();
//    if (drawPtr->passParameter__.depthAttachment__.has_value())
//    {
//      renderingInfo.pDepthAttachment = &drawPtr->passParameter__.depthAttachment__.value();
//    }
//    if (drawPtr->passParameter__.stencilAttachment__.has_value())
//    {
//      renderingInfo.pStencilAttachment = &drawPtr->passParameter__.stencilAttachment__.value();
//    }
//    renderingInfo.renderArea = {
//      0,
//      0,
//      gpu::ctx__->pSwapChainContext->extent__.width,
//      gpu::ctx__->pSwapChainContext->extent__.height
//      };
//    gpu::cmdBeginRendering(cmd, &renderingInfo);
//    Renderer->draw(cmd ,gpu::ctx__->renderingContext.currentFrame__);
//    gpu::cmdEndRendering(cmd);
//  };
//
//
//gpu::PassId test = gpu::ctx__->graphBuilder->addPass(pass);
//gpu::PassId drawTest = gpu::ctx__->graphBuilder->addPass(draw);
//gpu::ctx__->graphBuilder->addWriteResource(drawTest, swapchain);
//gpu::ctx__->graphBuilder->addResource(test, swapchain, swapchain);
