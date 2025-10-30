#include "render_target_film.hpp"

void flm::RenderTragetFilm::buildFrameResources()
{
  for (uint32_t i = 0; i < gpu::ctx__->renderingContext.maxInflight__; i++)
  {
    swapchainAttachment_.push_back(gpu::VkGraphBuilder::buildSwapchainAttachment(i));
    depthAttachmentHandle_.push_back(gpu::VkGraphBuilder::buildDepthAttachment());
    gBufferAlbedoHandle_.push_back(gpu::VkGraphBuilder::buildFrameAttachment(gpu::FORMAT_B8G8R8A8_SRGB));
    gBufferPositionHandle_.push_back(gpu::VkGraphBuilder::buildFrameAttachment(gpu::FORMAT_R16G16B16A16_SFLOAT));
    gBufferNormalHandle_.push_back(gpu::VkGraphBuilder::buildFrameAttachment(gpu::FORMAT_R16G16B16A16_SNORM));
    gBufferRoughnessHandle_.push_back(gpu::VkGraphBuilder::buildFrameAttachment(gpu::FORMAT_R16_UNORM));
    gBufferMetalicHandle_.push_back(gpu::VkGraphBuilder::buildFrameAttachment(gpu::FORMAT_R8_UNORM));
    lightningAttachmentHandle_.push_back(gpu::VkGraphBuilder::buildFrameAttachment(gpu::FORMAT_R16G16B16A16_SFLOAT));
    shadowAttachmentHandleXp_.push_back(gpu::VkGraphBuilder::buildDepthAttachment());
    shadowAttachmentHandleXm_.push_back(gpu::VkGraphBuilder::buildDepthAttachment());
    shadowAttachmentHandleYp_.push_back(gpu::VkGraphBuilder::buildDepthAttachment());
    shadowAttachmentHandleYm_.push_back(gpu::VkGraphBuilder::buildDepthAttachment());
    shadowAttachmentHandleZp_.push_back(gpu::VkGraphBuilder::buildDepthAttachment());
    shadowAttachmentHandleZm_.push_back(gpu::VkGraphBuilder::buildDepthAttachment());
    bloomingExtractAttachment_.push_back(gpu::VkGraphBuilder::buildFrameAttachment(gpu::FORMAT_R32G32B32A32_SFLOAT));
    bloomingBlurAttachment_.push_back(gpu::VkGraphBuilder::buildFrameAttachment(gpu::FORMAT_R32G32B32A32_SFLOAT));
    toneMappingAttachment_.push_back(gpu::VkGraphBuilder::buildFrameAttachment(gpu::FORMAT_R16G16B16A16_SFLOAT));
    gammaAttachment_.push_back(gpu::VkGraphBuilder::buildFrameAttachment(gpu::FORMAT_B8G8R8A8_SRGB));
  }
}

void flm::RenderTragetFilm::updateFrameConstant()
{
  this->renderAttachment.DepthBuffer =
    depthAttachmentHandle_[gpu::ctx__->renderingContext.currentFrame__]->descriptorArrayIndex__;
  this->renderAttachment.gBufferAlbedo =
    gBufferAlbedoHandle_[gpu::ctx__->renderingContext.currentFrame__]->descriptorArrayIndex__;
  this->renderAttachment.gBufferNormal =
    gBufferNormalHandle_[gpu::ctx__->renderingContext.currentFrame__]->descriptorArrayIndex__;
  this->renderAttachment.gBufferPositon =
    gBufferPositionHandle_[gpu::ctx__->renderingContext.currentFrame__]->descriptorArrayIndex__;
  this->renderAttachment.gBufferRoughness =
    gBufferRoughnessHandle_[gpu::ctx__->renderingContext.currentFrame__]->descriptorArrayIndex__;
  this->renderAttachment.lightningBuffer =
    lightningAttachmentHandle_[gpu::ctx__->renderingContext.currentFrame__]->descriptorArrayIndex__;
}

void flm::RenderTragetFilm::clearGbuffer()
{
  gBufferAlbedoHandle_[gpu::ctx__->renderingContext.currentFrame__]->lastWriter__ = nullptr;
  gBufferNormalHandle_[gpu::ctx__->renderingContext.currentFrame__]->lastWriter__ = nullptr;
  gBufferPositionHandle_[gpu::ctx__->renderingContext.currentFrame__]->lastWriter__ = nullptr;
  gBufferRoughnessHandle_[gpu::ctx__->renderingContext.currentFrame__]->lastWriter__ = nullptr;
  gBufferAlbedoHandle_[gpu::ctx__->renderingContext.currentFrame__]->writen__ = false;
  gBufferNormalHandle_[gpu::ctx__->renderingContext.currentFrame__]->writen__ = false;
  gBufferPositionHandle_[gpu::ctx__->renderingContext.currentFrame__]->writen__ = false;
  gBufferRoughnessHandle_[gpu::ctx__->renderingContext.currentFrame__]->writen__ = false;
}

void flm::RenderTragetFilm::clearDepthBuffer()
{
  depthAttachmentHandle_[gpu::ctx__->renderingContext.currentFrame__]->writen__ = false;
  depthAttachmentHandle_[gpu::ctx__->renderingContext.currentFrame__]->lastWriter__ = nullptr;
}

void flm::RenderTragetFilm::clearShadowBuffer()
{
  shadowAttachmentHandleXm_[gpu::ctx__->renderingContext.currentFrame__]->lastWriter__ = nullptr;
  shadowAttachmentHandleXm_[gpu::ctx__->renderingContext.currentFrame__]->writen__ = false;
  shadowAttachmentHandleYm_[gpu::ctx__->renderingContext.currentFrame__]->lastWriter__ = nullptr;
  shadowAttachmentHandleYm_[gpu::ctx__->renderingContext.currentFrame__]->writen__ = false;
  shadowAttachmentHandleZm_[gpu::ctx__->renderingContext.currentFrame__]->lastWriter__ = nullptr;
  shadowAttachmentHandleZm_[gpu::ctx__->renderingContext.currentFrame__]->writen__ = false;
  shadowAttachmentHandleXp_[gpu::ctx__->renderingContext.currentFrame__]->lastWriter__ = nullptr;
  shadowAttachmentHandleXp_[gpu::ctx__->renderingContext.currentFrame__]->writen__ = false;
  shadowAttachmentHandleYp_[gpu::ctx__->renderingContext.currentFrame__]->lastWriter__ = nullptr;
  shadowAttachmentHandleYp_[gpu::ctx__->renderingContext.currentFrame__]->writen__ = false;
  shadowAttachmentHandleZp_[gpu::ctx__->renderingContext.currentFrame__]->lastWriter__ = nullptr;
  shadowAttachmentHandleZp_[gpu::ctx__->renderingContext.currentFrame__]->writen__ = false;
}

void flm::RenderTragetFilm::clearLightBuffer()
{
  lightningAttachmentHandle_[gpu::ctx__->renderingContext.currentFrame__]->writen__ = false;
  lightningAttachmentHandle_[gpu::ctx__->renderingContext.currentFrame__]->lastWriter__ = nullptr;
}

void flm::RenderTragetFilm::clearBloomingBuffer()
{
  bloomingBlurAttachment_[gpu::ctx__->renderingContext.currentFrame__]->writen__ = false;
  bloomingBlurAttachment_[gpu::ctx__->renderingContext.currentFrame__]->lastWriter__ = nullptr;

  bloomingExtractAttachment_[gpu::ctx__->renderingContext.currentFrame__]->writen__ = false;
  bloomingExtractAttachment_[gpu::ctx__->renderingContext.currentFrame__]->lastWriter__ = nullptr;
}
