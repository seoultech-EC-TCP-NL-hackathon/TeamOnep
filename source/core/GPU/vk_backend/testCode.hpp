//
// Created by ljh on 25. 10. 6..
//

#ifndef _TESTCODE_HPP_
#define _TESTCODE_HPP_



// void GPU::VkGraphCompiler::compileRscUsage(VkNode *node)
// {
//   if (node->type__ == ResourceType::BUFFER) {
// 	// not allocate -> defautlt : GPU LOCALITY
// 	// -> staging buffer create and allocate
// 	// read: staging -> copy pass and transitition pass
// 	switch (node->usage) {
// 	  case (GPU::ResourceUsage::MESH_BUFFER): {
// 		VkFBuffer *vertexBuffer = reinterpret_cast<VkFBuffer *>(node);
// 		VkFBuffer stagingBuffer = buildBuffer(
// 			vertexBuffer->size__,
// 			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
// 			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
// 			VK_BUFFER_USAGE_TRANSFER_SRC_BIT
// 		);
// 		hostUpdate(
// 			stagingBuffer,
// 			vertexBuffer->size__,
// 			vertexBuffer->data__
// 		);
//
// 		VkPass copyPass = {};
// 		copyPass.passId__ = passId_;
// 		copyPass.read__ = {};
// 		copyPass.write__ = {node};
// 		copyPass.execute = [&vertexBuffer, &stagingBuffer](VkCommandBuffer cmd)
// 		{
// 		  VkDeviceSize offsets = 0;
// 		  VkBufferCopy bufferCopy{};
// 		  bufferCopy.srcOffset = 0;
// 		  bufferCopy.dstOffset = 0;
// 		  bufferCopy.size = vertexBuffer->size__;
// 		  vkCmdCopyBuffer(cmd, stagingBuffer.bufferh__, vertexBuffer->bufferh__, 1, &bufferCopy);
// 		};
// 		compiledPass.push_back(copyPass);
// 		node->lastPass__ = &copyPass;
// 		passId_++;
//
// 		VkPass barrierPass;
// 		barrierPass.passId__ = passId_;
// 		barrierPass.read__ = {node};
// 		barrierPass.write__ = {};
// 		barrierPass.execute = [&vertexBuffer, this](VkCommandBuffer cmd)
// 		{
// 		  VkDeviceSize offsets = 0;
// 		  VkBufferMemoryBarrier barrier{};
// 		  barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
// 		  barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
// 		  barrier.dstAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
// 		  barrier.srcQueueFamilyIndex = this->context_.graphicsFamailyIdx__;
// 		  barrier.dstQueueFamilyIndex = this->context_.graphicsFamailyIdx__;
// 		  barrier.buffer = vertexBuffer->bufferh__;
// 		  barrier.offset = offsets;
// 		  barrier.size = vertexBuffer->size__;
// 		  vkCmdPipelineBarrier(
// 			  cmd,
// 			  VK_PIPELINE_STAGE_TRANSFER_BIT,
// 			  VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
// 			  0,
// 			  0,
// 			  nullptr,
// 			  1,
// 			  &barrier,
// 			  0,
// 			  nullptr
// 		  );
// 		};
// 		compiledPass.push_back(barrierPass);
// 		passId_++;
// 		break;
//
// 		VkPass barrierPass;
// 		barrierPass.passId__ = passId_;
// 		barrierPass.read__ = {node};
// 		barrierPass.write__ = {};
// 		barrierPass.execute = [&indexBuffer, this](VkCommandBuffer cmd)
// 		{
// 		  VkDeviceSize offsets = 0;
// 		  VkBufferMemoryBarrier barrier{};
// 		  barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
// 		  barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
// 		  barrier.dstAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
// 		  barrier.srcQueueFamilyIndex = this->context_.graphicsFamailyIdx__;
// 		  barrier.dstQueueFamilyIndex = this->context_.graphicsFamailyIdx__;
// 		  barrier.buffer = indexBuffer->bufferh__;
// 		  barrier.offset = offsets;
// 		  barrier.size = indexBuffer->size__;
// 		  vkCmdPipelineBarrier(
// 			  cmd,
// 			  VK_PIPELINE_STAGE_TRANSFER_BIT,
// 			  VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
// 			  0,
// 			  0,
// 			  nullptr,
// 			  1,
// 			  &barrier,
// 			  0,
// 			  nullptr
// 		  );
// 		};
// 		compiledPass.push_back(barrierPass);
// 		passId_++;
// 		break;
// 	  }
//
// 	  case (GPU::ResourceUsage::UNIFORM_BUFFER): {
// 		VkFBuffer *buffer = reinterpret_cast<VkFBuffer *>(node);
// 		VkFBuffer builded = buildBuffer(
// 			buffer->size__,
// 			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
// 			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
// 			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
// 		);
// 		hostUpdate(
// 			builded,
// 			buffer->size__,
// 			buffer->data__
// 		);
// 		buffer = &builded;
// 		descriptorUploader_.writeUbo(buffer.);
// 		descriptorUploader_.update();
// 		passId_++;
// 		break;
// 	  }
// 	  case (GPU::ResourceUsage::SHADER_STORAGE_BUFFER): {
// 		///nda -> need to determine
// 		VkFBuffer *buffer = reinterpret_cast<VkFBuffer *>(node);
// 		buildBuffer(
// 			buffer->size__,
// 			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
// 			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
// 		);
// 	  }
// 	  default: {
// 		///indirect buffer
// 		///acceleation structure buffer update
// 		break;
// 	  }
// 	}
//   }
//   if (node->type__ == ResourceType::IMAGE) {
// 	switch (node->usage) {
// 	  case (GPU::ResourceUsage::G_Buffer): {
// 		VkImageCreateInfo imageInfo{};
// 		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
// 		imageInfo.imageType = VK_IMAGE_TYPE_2D;
// 		imageInfo.flags = 0;
// 		imageInfo.extent.width = image->width;
// 		imageInfo.extent.height = height;
// 		imageInfo.extent.depth = 1;
// 		imageInfo.mipLevels = 1;
// 		imageInfo.arrayLayers = 1;
// 		imageInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
// 		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL; //gpu optional
// 		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
// 		imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
// 		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
// 		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
// 		imageInfo.pNext = nullptr;
// 		vkCreateImage(device, &imageInfo, nullptr);
// 		break;
// 	  }
// 	  case (GPU::ResourceUsage::DEPTH_STENCIL_ATTACHMENT): {
// 		VkImageCreateInfo imageInfo{};
// 		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
// 		imageInfo.imageType = VK_IMAGE_TYPE_2D;
// 		imageInfo.flags = 0;
// 		imageInfo.extent.width = image->width;
// 		imageInfo.extent.height = height;
// 		imageInfo.extent.depth = 1;
// 		imageInfo.mipLevels = 1;
// 		imageInfo.arrayLayers = 1;
// 		imageInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
// 		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL; //gpu optional
// 		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
// 		imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
// 		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
// 		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
// 		imageInfo.pNext = nullptr;
// 		vkCreateImage(device, &imageInfo, nullptr);
//
// 		break;
// 	  }
// 	  case (GPU::ResourceUsage::TEXTURE): {
// 		VkFImage *image = reinterpret_cast<VkFImage *>(node);
// 		VkImageCreateInfo imageInfo{};
// 		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
// 		imageInfo.imageType = VK_IMAGE_TYPE_2D;
// 		imageInfo.flags = 0;
// 		imageInfo.extent.width = image->width;
// 		imageInfo.extent.height = height;
// 		imageInfo.extent.depth = 1;
// 		imageInfo.mipLevels = 1;
// 		imageInfo.arrayLayers = 1;
// 		imageInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
// 		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL; //gpu optional
// 		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
// 		imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
// 		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
// 		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
// 		imageInfo.pNext = nullptr;
//
// 		pAllocator_->createImage(
// 			*image,
// 			imageInfo,
// 			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
// 		);
// 		VkFBuffer buffer = buildBuffer(
// 			image->allocation__.size,
// 			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
// 			| VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
// 			VK_BUFFER_USAGE_TRANSFER_SRC_BIT
// 		);
//
// 		VkPass barrierPass2;
//
// 		VkImageMemoryBarrier barrier{};
// 		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
// 		barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
// 		barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
// 		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
// 		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
// 		barrier.image = textureImage;
// 		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
// 		barrier.subresourceRange.baseMipLevel = 0;
// 		barrier.subresourceRange.levelCount = level;
// 		barrier.subresourceRange.baseArrayLayer = 0;
// 		barrier.subresourceRange.layerCount = 1;
// 		barrier.srcAccessMask = 0;
// 		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
// 		vkCmdPipelineBarrier(
// 			command,
// 			VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
// 			VK_PIPELINE_STAGE_TRANSFER_BIT,
// 			0,
// 			0,
// 			nullptr,
// 			0,
// 			nullptr,
// 			1,
// 			&barrier
// 		);
//
// 		VkPass CopyBufferToImgPass;
// 		VkBufferImageCopy region{};
// 		region.bufferOffset = 0;
// 		region.bufferRowLength = 0;
// 		region.bufferImageHeight = 0;
// 		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
// 		region.imageSubresource.mipLevel = level;
// 		region.imageSubresource.baseArrayLayer = 0;
// 		region.imageSubresource.layerCount = 1;
// 		region.imageOffset = {0, 0, 0};
// 		region.imageExtent = {width, height, 1};
// // 		spdlog::info("call to command buffer to trenslate to texture");
// // 		vkCmdCopyBufferToImage(
// // 			command,
// // 			stagingBuffer->getStagingBuffer(),
// // 			textureImage,
// // 			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
// // 			1,
// // 			&region
// // 		);
// //
// // 		VkPass barrierPass;
// ////
// // Created by ljh on 25. 9. 27..
// //
//
//
// #include <backend_graph/render_graph.hpp>
//
// void VkGraph::setUp(VkCommandBuffer cmd)
// {
//   vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_h); // 파이프라인 바인딩
//   vkCmdSetPolygonModeEXT(cmd, polygonMode);
//   vkCmdSetDepthTestEnable(cmd, depthTest);
//   camera->camUpdate();
// }
//
// void VkGraph::draw(VkCommandBuffer cmd, uint32_t currenFrame)
// {
//   if (drawBackground)
//   {
//     vkCmdSetPolygonModeEXT(cmd, polygonMode);
//     vkCmdSetDepthTestEnable(cmd, depthTest);
//     vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, backgroundPipeline_);
//     viewport.x        = 0.0f;
//     viewport.y        = 0.0f;
//     viewport.width    = (float) swapchain__->getExtent().width;
//     viewport.height   = (float) swapchain__->getExtent().height;
//     viewport.minDepth = 0.0f;
//     viewport.maxDepth = 1.0f;
//     vkCmdSetViewport(cmd, 0, 1, &viewport);
//     scissor.offset = {0, 0};
//     scissor.extent = swapchain__->getExtent();
//     vkCmdSetScissor(cmd, 0, 1, &scissor);
//
//     vkCmdDraw(cmd, 6, 1, 0, 0);
//   }
//
//   vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_h);
//   vkCmdSetPolygonModeEXT(cmd, polygonMode);
//   vkCmdSetDepthTestEnable(cmd, depthTest);
//   switch (viewMode)
//   {
//     case (ViewMode::SINGLE):
//     {
//       pushConstant(cmd);
//       camera->camUpdate();
//       viewport.x        = 0.0f;
//       viewport.y        = 0.0f;
//       viewport.width    = (float) swapchain__->getExtent().width;
//       viewport.height   = (float) swapchain__->getExtent().height;
//       viewport.minDepth = 0.0f;
//       viewport.maxDepth = 1.0f;
//       vkCmdSetViewport(cmd, 0, 1, &viewport);
//
//       scissor.offset = {0, 0};
//       scissor.extent = swapchain__->getExtent();
//       vkCmdSetScissor(cmd, 0, 1, &scissor);
//
//       for (auto &mesh: pResourceManager.meshes_)
//       {
//         if (mesh.second == nullptr) return;
//         mesh.second->bind(cmd);
//         mesh.second->draw(cmd);
//       }
//       break;
//     }
//     case (ViewMode::MULTI):
//     {
//       auto extent      = swapchain__->getExtent();
//       float halfWidth  = extent.width / 2.0f;
//       float halfHeight = extent.height / 2.0f;
//       for (uint32_t i = 0; i < 4; i++)
//       {
//         viewport.width    = halfWidth;
//         viewport.height   = halfHeight;
//         viewport.minDepth = 0.0f;
//         viewport.maxDepth = 1.0f;
//
//         viewport.x = (i % 2) * halfWidth;
//         viewport.y = (i / 2) * halfHeight;
//         vkCmdSetViewport(cmd, 0, 1, &viewport);
//         pushConstant(cmd);
//         scissor.offset = {static_cast<int32_t>(viewport.x), static_cast<int32_t>(viewport.y)};
//         scissor.extent = {static_cast<uint32_t>(viewport.width), static_cast<uint32_t>(viewport.height)};
//         vkCmdSetScissor(cmd, 0, 1, &scissor);
//         for (auto &mesh: pResourceManager.meshes_)
//         {
//           mesh.second->bind(cmd);
//           mesh.second->draw(cmd);
//         }
//       }
//       break;
//     }
//     case (ViewMode::VR):
//     {
//       pushConstant(cmd);
//       camera->onVR(true);
//       pResourceManager.updateDescriptorSet(currenFrame);
//       viewport.x        = 0.0f;
//       viewport.y        = 0.0f;
//       viewport.width    = (float) swapchain__->getExtent().width / 2;
//       viewport.height   = (float) swapchain__->getExtent().height;
//       viewport.minDepth = 0.0f;
//       viewport.maxDepth = 1.0f;
//       vkCmdSetViewport(cmd, 0, 1, &viewport);
//
//       scissor.offset = {0, 0};
//       scissor.extent = swapchain__->getExtent();
//       vkCmdSetScissor(cmd, 0, 1, &scissor);
//
//       for (auto &mesh: pResourceManager.meshes_)
//       {
//         if (mesh.second == nullptr) return;
//         mesh.second->bind(cmd);
//         mesh.second->draw(cmd);
//       }
//       pushConstant(cmd);
//       camera->onVR(false);
//       pResourceManager.updateDescriptorSet(currenFrame);
//       viewport.x        = swapchain__->getExtent().width / 2;
//       viewport.y        = 0.0f;
//       viewport.width    = (float) swapchain__->getExtent().width / 2;
//       viewport.height   = (float) swapchain__->getExtent().height;
//       viewport.minDepth = 0.0f;
//       viewport.maxDepth = 1.0f;
//       vkCmdSetViewport(cmd, 0, 1, &viewport);
//
//       scissor.offset = {0, 0};
//       scissor.extent = swapchain__->getExtent();
//       vkCmdSetScissor(cmd, 0, 1, &scissor);
//
//       for (auto &mesh: pResourceManager.meshes_)
//       {
//         if (mesh.second == nullptr) return;
//         mesh.second->bind(cmd);
//         mesh.second->draw(cmd);
//       }
//       break;
//     }
//     default:
//       break;
//   }
// }
//
//
//
//
// //
// // Created by ljh on 25. 9. 16..
// //
//
// #include "dynamic_renderer.hpp"
// #include "renderer_resource.hpp"
// void DynamicRenderer::dynamicRendering()
// {
//
//
//   glfwPollEvents();
//
//   gpu::fps_cam camData = fpsCam_.flush();
//   std::memcpy(camBufHandle[sysRenderContext_.currentFrame].mapped, &camData, sizeof(gpu::fps_cam));
//
//   EventProcessor_->getKey();
//   EventProcessor_->wheelUpdate();
//
//   if (imageFence[sysRenderContext_.imageIndex] != VK_NULL_HANDLE)
//   {
//     vkWaitForFences(device_h,
//       1, &imageFence[sysRenderContext_.imageIndex], VK_TRUE, 10000000000);
//   }
//   inflightSyncManager_->wait(sysRenderContext_.currentFrame);
//   inflightSyncManager_->reset(sysRenderContext_.currentFrame);
//   VkSemaphore imageAvailable  = presentSyncManager_->getSingal(sysRenderContext_.currentFrame);
//   VkSemaphore renderFinishied = renderSyncManager_->getSingal(sysRenderContext_.currentFrame);
//   VkResult result             = vkAcquireNextImageKHR(device_h,
//                                           swapchain_->getSwapchain(),
//                                           100000,
//                                           imageAvailable,
//                                           VK_NULL_HANDLE,
//                                           &sysRenderContext_.imageIndex);
//   if (result == VK_ERROR_OUT_OF_DATE_KHR)
//   {
//     swapchain_->recreateSwapchain();
//     return;
//   } else if ((result != VK_SUCCESS) && (result != VK_SUBOPTIMAL_KHR))
//   {
//     throw std::runtime_error("Could not acquire the next swap chain image!");
//   }
//   VkCommandBuffer commandBuffer = commandManager_->beginPrimary(sysRenderContext_.currentFrame);
//
//   VkImageSubresourceRange colorSubresourceRange = {};
//   colorSubresourceRange.aspectMask              = VK_IMAGE_ASPECT_COLOR_BIT;
//   colorSubresourceRange.baseMipLevel            = 0;
//   colorSubresourceRange.levelCount              = 1;
//   colorSubresourceRange.layerCount              = 1;
//
//   streamingBuffer_->recordImageBarrier(commandBuffer,
//                                        swapchain_->getImage(sysRenderContext_.imageIndex),
//                                        colorSubresourceRange,
//                                        0,
//                                        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
//                                        VK_IMAGE_LAYOUT_UNDEFINED,
//                                        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
//                                        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
//                                        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
//
//   VkImageSubresourceRange depthSubresourceRange = {};
//   depthSubresourceRange.aspectMask              = VK_IMAGE_ASPECT_DEPTH_BIT;
//   depthSubresourceRange.baseMipLevel            = 0;
//   depthSubresourceRange.levelCount              = 1;
//   depthSubresourceRange.layerCount              = 1;
//
//   streamingBuffer_->recordImageBarrier(commandBuffer,
//                                        frameBuffer_->getDepthImage(sysRenderContext_.imageIndex),
//                                        depthSubresourceRange,
//                                        0,
//                                        VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
//                                        VK_IMAGE_LAYOUT_UNDEFINED,
//                                        VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
//                                        VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
//                                        VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
//                                        VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
//                                        VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT);
//
//   VkRenderingAttachmentInfo colorAttachment{};
//   colorAttachment.sType            = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
//   colorAttachment.imageView        = swapchain_->getSwapchainImageView(sysRenderContext_.imageIndex);
//   colorAttachment.imageLayout      = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
//   colorAttachment.loadOp           = VK_ATTACHMENT_LOAD_OP_CLEAR;
//   colorAttachment.storeOp          = VK_ATTACHMENT_STORE_OP_STORE;
//   colorAttachment.clearValue.color = {0.0f, 0.0f, 0.0f, 1.0};
//
//   VkRenderingAttachmentInfo depthAttachment{};
//   depthAttachment.sType                   = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
//   depthAttachment.imageView               = frameBuffer_->getDepthView(sysRenderContext_.imageIndex);
//   depthAttachment.imageLayout             = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
//   depthAttachment.loadOp                  = VK_ATTACHMENT_LOAD_OP_CLEAR;
//   depthAttachment.storeOp                 = VK_ATTACHMENT_STORE_OP_DONT_CARE;
//   depthAttachment.clearValue.depthStencil = {1.0f, 0};
//
//   VkRenderingInfo renderingInfo{VK_STRUCTURE_TYPE_RENDERING_INFO_KHR};
//   renderingInfo.layerCount           = 1;
//   renderingInfo.colorAttachmentCount = 1;
//   renderingInfo.pColorAttachments    = &colorAttachment;
//   renderingInfo.pDepthAttachment     = &depthAttachment;
//   renderingInfo.pStencilAttachment   = nullptr;
//   renderingInfo.renderArea           = {
//   0,
//   0,
//   swapchain_->getExtent().width,
//   swapchain_->getExtent().height
//   };
//   VkExtent2D swapExtent = swapchain_->getExtent();
//   vkCmdBeginRendering(commandBuffer, &renderingInfo);
//
//
//   vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, sysScenePipeline_h);
//   vkCmdBindDescriptorSets(commandBuffer,
//                           VK_PIPELINE_BIND_POINT_GRAPHICS,
//                           sysPipelineLayout_h,
//                           0,
//                           1,
//                           &sysBindlessDescriptor_h,
//                           0,
//                           nullptr);
//
//   vkCmdBindDescriptorSets(commandBuffer,
//                           VK_PIPELINE_BIND_POINT_GRAPHICS,
//                           sysPipelineLayout_h,
//                           0,
//                           1,
//                           &camBufHandle[sysRenderContext_.currentFrame].descriptorSet,
//                           0,
//                           nullptr);
//   VkViewport viewport{
//   0.0f,
//   0.0f,
//   (float) swapExtent.width,
//   (float) swapExtent.height,
//   0.0f,
//   1.0f
//   };
//
//   vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
//   VkRect2D scissor{
//   0,
//   0,
//   swapExtent.width,
//   swapExtent.height
//   };
//   vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
//
//   VkDeviceSize offsets[1]{0};
//
//   vkCmdBindVertexBuffers(commandBuffer,
//                          0,
//                          1,
//                          &batches[0].vertexBuffer,
//                          offsets);
//
//   vkCmdBindIndexBuffer(commandBuffer,
//                        batches[0].indexBuffer,
//                        0,
//                        VK_INDEX_TYPE_UINT32);
//
//     vkCmdDrawIndexed(commandBuffer,
//                      batches[0].indexCount,
//                      1,
//                      0,
//                      0,
//                      0);
//
//   vkCmdEndRendering(commandBuffer);
//
//   VkImageSubresourceRange renderResourceRange = {
//   VK_IMAGE_ASPECT_COLOR_BIT,
//   0,
//   1,
//   0,
//   1
//   };
//
//   streamingBuffer_->recordImageBarrier(commandBuffer,
//                                        swapchain_->getImage(sysRenderContext_.imageIndex),
//                                        renderResourceRange,
//                                        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
//                                        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
//                                        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
//                                        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
//                                        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
//                                        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
//     );
//
//   VkRenderPassBeginInfo renderPassBeginInfo{};
//   renderPassBeginInfo.sType                    = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
//   renderPassBeginInfo.renderPass               = sysUiPass_h;
//   renderPassBeginInfo.renderArea.offset.x      = 0;
//   renderPassBeginInfo.renderArea.offset.y      = 0;
//   renderPassBeginInfo.renderArea.extent.width  = swapExtent.width;
//   renderPassBeginInfo.renderArea.extent.height = swapExtent.height;
//   renderPassBeginInfo.clearValueCount          = 0;
//   renderPassBeginInfo.pClearValues             = {0};
//   renderPassBeginInfo.framebuffer              = sysRenderContext_.frameBuffers[sysRenderContext_.currentFrame];
//
//   vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
//   vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, sysScenePipeline_h);
//   vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
//   vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
//   uiRenderer_->draw(commandBuffer, *controler_);
//   vkCmdEndRenderPass(commandBuffer);
//   commandManager_->end(commandBuffer);
//
//   VkPipelineStageFlags waitStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
//   VkSubmitInfo submitInfo{VK_STRUCTURE_TYPE_SUBMIT_INFO};
//   submitInfo.pWaitDstStageMask    = &waitStageMask;
//   submitInfo.pCommandBuffers      = &commandBuffer;
//   submitInfo.commandBufferCount   = 1;
//   submitInfo.pWaitSemaphores      = &imageAvailable;
//   submitInfo.waitSemaphoreCount   = 1;
//   submitInfo.pSignalSemaphores    = &renderFinishied;
//   submitInfo.signalSemaphoreCount = 1;
//   VkFence waitFence               = inflightSyncManager_->get(sysRenderContext_.currentFrame);
//
//   imageFence[sysRenderContext_.imageIndex] = waitFence;
//   VK_ASSERT(vkQueueSubmit(vkContext_->graphics_q, 1, &submitInfo, waitFence));
//   VkSwapchainKHR swapchain_h = (swapchain_->getSwapchain());
//   VkPresentInfoKHR presentInfo{VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
//   presentInfo.waitSemaphoreCount = 1;
//   presentInfo.pWaitSemaphores    = &renderFinishied;
//   presentInfo.swapchainCount     = 1;
//   presentInfo.pSwapchains        = &swapchain_h;
//   presentInfo.pImageIndices      = &sysRenderContext_.imageIndex;
//   result                         = vkQueuePresentKHR(vkContext_->present_q, &presentInfo);
//   if ((result == VK_ERROR_OUT_OF_DATE_KHR) || (result == VK_SUBOPTIMAL_KHR))
//   {
//     swapchain_->recreateSwapchain();
//     ///todo ::recreate depth image
//   } else if (result != VK_SUCCESS)
//   {
//     throw "Could not present the image to the swap chain!";
//   }
//   sysRenderContext_.currentFrame = (sysRenderContext_.currentFrame + 1) % sysRenderContext_.MAX_FRAMES_IN_FLIGHT;
// }
//
// void GPU::VkGraph::renderingEnd()
// {
//   //swapchain showing image -> only care the color presentation
//   GPU::RenderPass renderingEnd{};
//   renderingEnd.id = castNode(GPU::RenderNodeType::RENDER_END);
//   renderingEnd.dependencies_ = {castNode(GPU::RenderNodeType::PIPELINE_INIT)};
//   renderingEnd.dependent_ = castNode(GPU::RenderNodeType::PRESENT_IMAGE);
//   renderingEnd.execute_ = [this]()
//   {
// 	vkCmdEndRendering(this->register_.cmd__);
// 	spdlog::debug("Render graph end");
// 	VkImageSubresourceRange colorSubresourceRange = {};
// 	colorSubresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
// 	colorSubresourceRange.baseMipLevel = 0;
// 	colorSubresourceRange.levelCount = 1;
// 	colorSubresourceRange.layerCount = 1;
//
// 	VkImageMemoryBarrier imageMemoryBarrier{};
// 	imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
// 	imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
// 	imageMemoryBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
// 	imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
// 	imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
// 	imageMemoryBarrier.image = this->swapchain_->img__[register_.imageIndex__];
// 	imageMemoryBarrier.subresourceRange = colorSubresourceRange;
// 	imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
// 	imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
//
// 	vkCmdPipelineBarrier(this->register_.cmd__,
// 						 VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
// 						 VK_IMAGE_ASPECT_COLOR_BIT,
// 						 0,
// 						 0,
// 						 nullptr,
// 						 0,
// 						 nullptr,
// 						 1,
// 						 &imageMemoryBarrier);
// 	if (vkEndCommandBuffer(this->register_.cmd__) != VK_SUCCESS) {
// 	  spdlog::error("Command buffer recording failed");
// 	  throw std::runtime_error("Command buffer recording failed");
// 	}
//   };
//   registerPass(renderingEnd);
// }
// 		break;
// 	  }
// 	  case (GPU::ResourceUsage::TEXTURE_CUBE): {
// 		break;
// 	  }
// 	  default: {
// 	  }
// 		break;
// 	}
//   }
// }
///
///
//GPU::Pool::SwapchainAttachmentPool(const DepthImageCI &info) : device(info.device),
//                                                           allocator(*info.allocator),
//                                                           extent(info.extent)
//{
//  GBuffer_depth__.resize(info.swapchainImageCount);
//  roughnessViews__.resize(info.swapchainImageCount);
//  for (int i = 0; i < GBuffer_depth__.size(); i++)
//  {
//    VkImageCreateInfo depthInfo{};
//    depthInfo.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
//    depthInfo.imageType     = VK_IMAGE_TYPE_2D;
//    depthInfo.extent.width  = extent.width;
//    depthInfo.extent.height = extent.height;
//    depthInfo.extent.depth  = 1;
//    depthInfo.mipLevels     = 1;
//    depthInfo.arrayLayers   = 1;
//    depthInfo.format        = VK_FORMAT_D32_SFLOAT; // 깊이 포맷
//    depthInfo.tiling        = VK_IMAGE_TILING_OPTIMAL;
//    depthInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
//    depthInfo.usage         = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT; // 깊이 스텐실 용도
//    depthInfo.samples       = VK_SAMPLE_COUNT_1_BIT;
//    depthInfo.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;
//    if (vkCreateImage(device, &depthInfo, nullptr, &GBuffer_depth__[i]) != VK_SUCCESS)
//    {
//      throw std::runtime_error("failed to create depth image!");
//    }
//    allocations[i] = memoryBind(i);
//    if (vkBindImageMemory(device, GBuffer_depth__[i], allocations[i].memory__, allocations[i].offset) != VK_SUCCESS)
//    {
//      throw std::runtime_error("fail to allocate depth memroy");
//    }
//  }
//  for (int i = 0; i < roughnessViews__.size(); i++)
//  {
//    VkImageViewCreateInfo depthInfo{};
//    depthInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
//    depthInfo.image                           = GBuffer_depth__[i];
//    depthInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
//    depthInfo.format                          = VK_FORMAT_D32_SFLOAT;
//    depthInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_DEPTH_BIT;
//    depthInfo.subresourceRange.baseMipLevel   = 0;
//    depthInfo.subresourceRange.levelCount     = 1;
//    depthInfo.subresourceRange.baseArrayLayer = 0;
//    depthInfo.subresourceRange.layerCount     = 1;
//    if (vkCreateImageView(device, &depthInfo, nullptr, &roughnessViews__[i]) != VK_SUCCESS)
//    {
//      throw std::runtime_error("fail to make depth View");
//    }
//  }
//}
//
//GPU::SwapchainAttachmentPool::~ SwapchainDepth()
//{
//  for (auto view: roughnessViews__)
//  {
//    vkDestroyImageView(device, view, nullptr);
//  }
//
//  for (auto img: GBuffer_depth__)
//  {
//    if (img != VK_NULL_HANDLE)
//    {
//      vkDestroyImage(device, img, nullptr);
//    }
//  }
//  spdlog::info("terminate imageView");
//  return;
//}
//

//Allocation GPU::SwapchainAttachmentPool::memoryBind(uint32_t index)
//{
//  VkMemoryRequirements memRequirements;
//  VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
//  vkGetImageMemoryRequirements(device, GBuffer_depth__[index], &memRequirements);
//  return allocator.allocate(memRequirements, properties);
//}
#endif //_TESTCODE_HPP_
