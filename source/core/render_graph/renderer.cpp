
#include "../GPU/context.hpp"
#include "renderer.hpp"

RenderingSystem::RenderingSystem(RenderInitInfo info) : device_h(info.device_h),
                                                        renderpass_h(info.renderPass),
                                                        pResourceManager(*info.resourceManager),
                                                        pDescriptorSetLayouts(info.pDescriptorSetLayouts),
                                                        descriptorLayoutCount(info.descriptorSetLayoutCount)
{
  vkCmdSetPolygonModeEXT = (PFN_vkCmdSetPolygonModeEXT) vkGetDeviceProcAddr(gpu::ctx__.deviceh__ , "vkCmdSetPolygonModeEXT");
  if (!vkCmdSetPolygonModeEXT)
  {
    throw std::runtime_error("vkCmdSetPolygonModeEXT not available!");
  }
  VkShaderModule vShader = pResourceManager.fragShaderPool->getShader(vertPath, shaderc_vertex_shader);
  VkShaderModule fshader = pResourceManager.fragShaderPool->getShader(fragPath, shaderc_fragment_shader);

  pipelineLayout_h = pResourceManager.pipelinePool_->createPipelineLayout(pDescriptorSetLayouts, descriptorLayoutCount);
  PipelineProgram program{};
  VkFormat swapchainfFormat      = gpu::ctx__.pSwapChainContext->imgFormat__;
  program.pColorAttachmentFormat = &swapchainfFormat;
  program.renderPass             = renderpass_h;
  program.pipelineLayout         = pipelineLayout_h;
  program.vertexType             = VertexType::ALL;
  program.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  program.vertShaderModule       = vShader;
  program.fragShaderModule       = fshader;
  pipeline_h                     = pResourceManager.pipelinePool_->createPipeline(program);

  VkShaderModule vBackground = pResourceManager.vertexShaderPool->getShader(VertBackPath, shaderc_vertex_shader);
  VkShaderModule fBackground = pResourceManager.fragShaderPool->getShader(fragBackPath, shaderc_fragment_shader);

  program.vertShaderModule = vBackground;
  program.fragShaderModule = fBackground;
  program.vertexType       = VertexType::BACKGROUND;
  backgroundPipeline_      = pResourceManager.pipelinePool_->createPipeline(program);
  drawBackground           = VK_TRUE;
}

void RenderingSystem::pushConstant(VkCommandBuffer command)
{
  vkCmdPushConstants(
                     command,
                     pipelineLayout_h,
                     VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                     0,
                     sizeof(MaterialConstant),
                     &pResourceManager.selectedModel.constant
                    );
  //spdlog::trace("albedo index: {} ",pResourceManager.selectedModel.constant.albedoTextureIndex) ;
  //spdlog::trace("metalic index: {} ",pResourceManager.selectedModel.constant.metalicTextureIndex) ;
  //spdlog::trace("normal index: {} ",pResourceManager.selectedModel.constant.normalTextureIndex) ;
}

void RenderingSystem::setUp(VkCommandBuffer cmd)
{
  vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_h); // 파이프라인 바인딩
  vkCmdSetPolygonModeEXT(cmd, polygonMode);
  vkCmdSetDepthTestEnable(cmd, depthTest);
  camera->camUpdate();
  viewport.x        = 0.0f;
  viewport.y        = 0.0f;
  viewport.width    = (float) gpu::ctx__.pSwapChainContext->extent__.width;
  viewport.height   = (float) gpu::ctx__.pSwapChainContext->extent__.height;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  vkCmdSetViewport(cmd, 0, 1, &viewport);
  scissor.offset = {0, 0};
  scissor.extent = gpu::ctx__.pSwapChainContext->extent__;
  vkCmdSetScissor(cmd, 0, 1, &scissor);
}

void RenderingSystem::draw(VkCommandBuffer cmd, uint32_t currenFrame)
{
  if (drawBackground)
  {
    vkCmdSetPolygonModeEXT(cmd, polygonMode);
    vkCmdSetDepthTestEnable(cmd, depthTest);
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, backgroundPipeline_);
    viewport.x        = 0.0f;
    viewport.y        = 0.0f;
    viewport.width    = (float) gpu::ctx__.pSwapChainContext->extent__.width;
    viewport.height   = (float) gpu::ctx__.pSwapChainContext->extent__.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(cmd, 0, 1, &viewport);
    scissor.offset = {0, 0};
    scissor.extent = gpu::ctx__.pSwapChainContext->extent__;
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
      camera->camUpdate();
      viewport.x        = 0.0f;
      viewport.y        = 0.0f;
      viewport.width    = (float) gpu::ctx__.pSwapChainContext->extent__.width;
      viewport.height   = (float) gpu::ctx__.pSwapChainContext->extent__.height;
      viewport.minDepth = 0.0f;
      viewport.maxDepth = 1.0f;
      vkCmdSetViewport(cmd, 0, 1, &viewport);

      scissor.offset = {0, 0};
      scissor.extent = gpu::ctx__.pSwapChainContext->extent__;
      vkCmdSetScissor(cmd, 0, 1, &scissor);

      for (auto &mesh: pResourceManager.meshes_)
      {
        if (mesh.second == nullptr) return;
        mesh.second->bind(cmd);
        mesh.second->draw(cmd);
      }
      break;
    }
    case (ViewMode::MULTI):
    {
      auto extent      = gpu::ctx__.pSwapChainContext->extent__;
      float halfWidth  = extent.width / 2.0f;
      float halfHeight = extent.height / 2.0f;
      for (uint32_t i = 0; i < 4; i++)
      {
        viewport.width    = halfWidth;
        viewport.height   = halfHeight;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        viewport.x = (i % 2) * halfWidth;
        viewport.y = (i / 2) * halfHeight;
        vkCmdSetViewport(cmd, 0, 1, &viewport);
        pushConstant(cmd);
        scissor.offset = {static_cast<int32_t>(viewport.x), static_cast<int32_t>(viewport.y)};
        scissor.extent = {static_cast<uint32_t>(viewport.width), static_cast<uint32_t>(viewport.height)};
        vkCmdSetScissor(cmd, 0, 1, &scissor);
        for (auto &mesh: pResourceManager.meshes_)
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
      camera->onVR(true);
      pResourceManager.updateDescriptorSet(currenFrame);
      viewport.x        = 0.0f;
      viewport.y        = 0.0f;
      viewport.width    = (float) gpu::ctx__.pSwapChainContext->extent__.width / 2;
      viewport.height   = (float) gpu::ctx__.pSwapChainContext->extent__.height;
      viewport.minDepth = 0.0f;
      viewport.maxDepth = 1.0f;
      vkCmdSetViewport(cmd, 0, 1, &viewport);

      scissor.offset = {0, 0};
      scissor.extent = gpu::ctx__.pSwapChainContext->extent__;
      vkCmdSetScissor(cmd, 0, 1, &scissor);

      for (auto &mesh: pResourceManager.meshes_)
      {
        if (mesh.second == nullptr) return;
        mesh.second->bind(cmd);
        mesh.second->draw(cmd);
      }
      pushConstant(cmd);
      camera->onVR(false);
      pResourceManager.updateDescriptorSet(currenFrame);
      viewport.x        = gpu::ctx__.pSwapChainContext->extent__.width / 2;
      viewport.y        = 0.0f;
      viewport.width    = (float) gpu::ctx__.pSwapChainContext->extent__.width / 2;
      viewport.height   = (float) gpu::ctx__.pSwapChainContext->extent__.height;
      viewport.minDepth = 0.0f;
      viewport.maxDepth = 1.0f;
      vkCmdSetViewport(cmd, 0, 1, &viewport);

      scissor.offset = {0, 0};
      scissor.extent = gpu::ctx__.pSwapChainContext->extent__;
      vkCmdSetScissor(cmd, 0, 1, &scissor);

      for (auto &mesh: pResourceManager.meshes_)
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
//      gpu::ctx__.pSwapChainContext->extent__.width,
//      gpu::ctx__.pSwapChainContext->extent__.height
//      };
//    gpu::cmdBeginRendering(cmd, &renderingInfo);
//    Renderer->draw(cmd ,gpu::ctx__.renderingContext.currentFrame__);
//    gpu::cmdEndRendering(cmd);
//  };
//
//
//gpu::PassId test = gpu::ctx__.pGraphBuilder->addPass(pass);
//gpu::PassId drawTest = gpu::ctx__.pGraphBuilder->addPass(draw);
//gpu::ctx__.pGraphBuilder->addWriteResource(drawTest, swapchain);
//gpu::ctx__.pGraphBuilder->addResource(test, swapchain, swapchain);