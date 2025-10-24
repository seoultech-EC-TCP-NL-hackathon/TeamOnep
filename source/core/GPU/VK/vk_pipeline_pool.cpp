#include "vk_pipeline_pool.hpp"
#include "vk_context.hpp"
#include "vk_vertex_attr.hpp"

namespace gpu
{
  VkPipelinePool::VkPipelinePool(VkContext* pCtxt) :
    pCtxt_(pCtxt)

  {
    device_ = pCtxt->deviceh__;
    std::vector<uint8_t> oldCacheData = loadPipelineCache("check this");

    VkPipelineCacheCreateInfo cacheInfo{};
    cacheInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    cacheInfo.initialDataSize = oldCacheData.size();
    cacheInfo.pInitialData = oldCacheData.empty() ? nullptr : oldCacheData.data();
    vkCreatePipelineCache(device_, &cacheInfo, nullptr, &oldPipelineCache_);
  }

  VkPipelinePool::~VkPipelinePool()
  {
    size_t dataSize = 0;
    vkGetPipelineCacheData(device_, oldPipelineCache_, &dataSize, nullptr);
    std::vector<uint8_t> cacheData(dataSize);
    vkGetPipelineCacheData(device_, oldPipelineCache_, &dataSize, cacheData.data());
    std::ofstream outFile(std::string(PIPELINE_CACHE_DIR),
                          std::ios::binary);

    outFile.write(reinterpret_cast<char*>(cacheData.data()), cacheData.size());
    for (auto pipeline : pipelineHash_)
    {
      if (pipeline.second != VK_NULL_HANDLE)
      {
        vkDestroyPipeline(device_, pipeline.second, nullptr);
      }
    }
    spdlog::info("destroy pipeline");
  }

  VkPipeline VkPipelinePool::createPipeline(VkPipelineProgram program)
  {
    VkPipeline pipeline = getPipeline(program);
    if (pipeline == VK_NULL_HANDLE)
    {
      return createPipeline(program.vertexType,
                            program.vertShaderModule,
                            program.fragShaderModule,
                            program.renderingType,
                            program.pipelineLayout);
    }
    return pipeline;
  }

  VkPipeline VkPipelinePool::getPipeline(
    VkPipelineProgram program
    ) const
  {
    auto it = pipelineHash_.find(program);
    if (it != pipelineHash_.end())
      return it->second;
    return VK_NULL_HANDLE;
  }

  void VkPipelinePool::createComputePipeline(
    const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts,
    VkShaderModule computeShader
    )
  {
    //push constant setting
    VkPushConstantRange pushConstant{};
    pushConstant.offset = 0;
    //pushConstant.size       = sizeof(gpu::constant);
    pushConstant.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    //if need -> create and layout setting
    VkPipelineLayoutCreateInfo computeLayout{};
    computeLayout.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    computeLayout.pNext = nullptr;
    computeLayout.pSetLayouts = descriptorSetLayouts.data();
    computeLayout.setLayoutCount = descriptorSetLayouts.size();
    computeLayout.pPushConstantRanges = &pushConstant;
    computeLayout.pushConstantRangeCount = 1;
    computeLayout.pPushConstantRanges = &pushConstant;
    computeLayout.pushConstantRangeCount = 1;

    if (vkCreatePipelineLayout(device_, &computeLayout, nullptr, &computePipelineLayout_) != VK_SUCCESS)
    {
      throw std::runtime_error("devicecreate pipeline layout!");
    }
    VkPipelineShaderStageCreateInfo shaderStageInfo{};
    shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    shaderStageInfo.module = computeShader;
    shaderStageInfo.pName = "main";

    VkComputePipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    pipelineInfo.stage = shaderStageInfo;
    pipelineInfo.layout = computePipelineLayout_;
    pipelineInfo.flags = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineInfo.basePipelineIndex = -1;
  }

  void VkPipelinePool::buildVertexDescriptor(VertexType type,
                                             VkVertexInputBindingDescription& vertexBindingDesc,
                                             std::vector<VkVertexInputAttributeDescription>&
                                             vertexAttributeDescriptions,
                                             VkPipelineVertexInputStateCreateInfo& vertexInputInfo,
                                             uint32_t vertexBinding)
  {
    switch (type)
    {
      case (gpu::VertexType::ALL):
      {
        vertexBindingDesc.binding = 0;
        vertexBindingDesc.stride = sizeof(gpu::VertexAll);
        vertexBindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        vertexAttributeDescriptions.resize(8);
        vertexAttributeDescriptions[0].binding = vertexBinding;
        vertexAttributeDescriptions[0].location = 0;
        vertexAttributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        vertexAttributeDescriptions[0].offset = offsetof(gpu::VertexAll, position);

        vertexAttributeDescriptions[1].binding = vertexBinding;
        vertexAttributeDescriptions[1].location = 1;
        vertexAttributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        vertexAttributeDescriptions[1].offset = offsetof(gpu::VertexAll, normal);

        vertexAttributeDescriptions[2].binding = vertexBinding;
        vertexAttributeDescriptions[2].location = 2;
        vertexAttributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        vertexAttributeDescriptions[2].offset = offsetof(gpu::VertexAll, uv);

        vertexAttributeDescriptions[3].binding = vertexBinding;
        vertexAttributeDescriptions[3].location = 3;
        vertexAttributeDescriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
        vertexAttributeDescriptions[3].offset = offsetof(gpu::VertexAll, tangent);

        vertexAttributeDescriptions[4].binding = vertexBinding;
        vertexAttributeDescriptions[4].location = 4;
        vertexAttributeDescriptions[4].format = VK_FORMAT_R32G32B32_SFLOAT;
        vertexAttributeDescriptions[4].offset = offsetof(gpu::VertexAll, bitangent);

        vertexAttributeDescriptions[5].binding = vertexBinding;
        vertexAttributeDescriptions[5].location = 5;
        vertexAttributeDescriptions[5].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        vertexAttributeDescriptions[5].offset = offsetof(gpu::VertexAll, color);

        vertexAttributeDescriptions[6].binding = vertexBinding;
        vertexAttributeDescriptions[6].location = 6;
        vertexAttributeDescriptions[6].format = VK_FORMAT_R32G32B32A32_SINT;
        vertexAttributeDescriptions[6].offset = offsetof(gpu::VertexAll, boneIndices);

        vertexAttributeDescriptions[7].binding = vertexBinding;
        vertexAttributeDescriptions[7].location = 7;
        vertexAttributeDescriptions[7].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        vertexAttributeDescriptions[7].offset = offsetof(gpu::VertexAll, boneWeights);
        break;
      }
      case (VertexType::PC):
      {
        vertexBindingDesc.binding = vertexBinding;
        vertexBindingDesc.stride = sizeof(VertexPC);
        vertexBindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        vertexAttributeDescriptions.resize(2);
        vertexAttributeDescriptions[0].binding = vertexBinding;
        vertexAttributeDescriptions[0].location = 0;
        vertexAttributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        vertexAttributeDescriptions[0].offset = offsetof(VertexPC, position);

        vertexAttributeDescriptions[1].binding = vertexBinding;
        vertexAttributeDescriptions[1].location = 1;
        vertexAttributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        vertexAttributeDescriptions[1].offset = offsetof(VertexPC, color);
        break;
      }
      case (VertexType::PUVN):
      {
        vertexBindingDesc.binding = vertexBinding;
        vertexBindingDesc.stride = sizeof(VertexPUVN);
        vertexBindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        vertexAttributeDescriptions.resize(3);
        vertexAttributeDescriptions[0].binding = vertexBinding;
        vertexAttributeDescriptions[0].location = 0;
        vertexAttributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        vertexAttributeDescriptions[0].offset = offsetof(VertexPUVN, position);

        vertexAttributeDescriptions[1].binding = vertexBinding;
        vertexAttributeDescriptions[1].location = 1;
        vertexAttributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
        vertexAttributeDescriptions[1].offset = offsetof(VertexPUVN, uv);

        vertexAttributeDescriptions[2].binding = vertexBinding;
        vertexAttributeDescriptions[2].location = 2;
        vertexAttributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
        vertexAttributeDescriptions[2].offset = offsetof(VertexPUVN, normal);
        break;
      }
      case (VertexType::PUVNTC):
      {
        vertexBindingDesc.binding = vertexBinding;
        vertexBindingDesc.stride = sizeof(VertexPUVNTC);
        vertexBindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        vertexAttributeDescriptions.resize(5);

        vertexAttributeDescriptions[0].binding = vertexBinding;
        vertexAttributeDescriptions[0].location = 0;
        vertexAttributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        vertexAttributeDescriptions[0].offset = offsetof(VertexPUVNTC, position);

        vertexAttributeDescriptions[1].binding = vertexBinding;
        vertexAttributeDescriptions[1].location = 1;
        vertexAttributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
        vertexAttributeDescriptions[1].offset = offsetof(VertexPUVNTC, uv);

        vertexAttributeDescriptions[2].binding = vertexBinding;
        vertexAttributeDescriptions[2].location = 2;
        vertexAttributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
        vertexAttributeDescriptions[2].offset = offsetof(VertexPUVNTC, normal);

        vertexAttributeDescriptions[3].binding = vertexBinding;
        vertexAttributeDescriptions[3].location = 3;
        vertexAttributeDescriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
        vertexAttributeDescriptions[3].offset = offsetof(VertexPUVNTC, tangent);

        vertexAttributeDescriptions[4].binding = vertexBinding;
        vertexAttributeDescriptions[4].location = 4;
        vertexAttributeDescriptions[4].format = VK_FORMAT_R32G32B32_SFLOAT;
        vertexAttributeDescriptions[4].offset = offsetof(VertexPUVNTC, color);
        break;
      }
      case (VertexType::QUAD):
      {
        vertexBindingDesc.binding = vertexBinding;
        vertexBindingDesc.stride = sizeof(Quad);
        vertexBindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        vertexAttributeDescriptions.resize(2);

        vertexAttributeDescriptions[0].binding = vertexBinding;
        vertexAttributeDescriptions[0].location = 0;
        vertexAttributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        vertexAttributeDescriptions[0].offset = offsetof(Quad, point1);

        vertexAttributeDescriptions[1].binding = vertexBinding;
        vertexAttributeDescriptions[1].location = 0;
        vertexAttributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
        vertexAttributeDescriptions[1].offset = offsetof(Quad, point2);
      }
      case (VertexType::BACKGROUND):
      {
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 0;
        vertexInputInfo.vertexAttributeDescriptionCount = 0;
        vertexInputInfo.pVertexBindingDescriptions = nullptr;
        vertexInputInfo.pVertexAttributeDescriptions = nullptr;
        return;
      }
      default:
      {
        break;
      }
    }
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &vertexBindingDesc;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexAttributeDescriptions.size());
    vertexInputInfo.pVertexAttributeDescriptions = vertexAttributeDescriptions.data();
  }

  void VkPipelinePool::buildDepthStencilPipeline(VkPipelineDepthStencilStateCreateInfo& depthStencilCi,
                                                 VkBool32 depthTestEnable,
                                                 VkBool32 depthWriteEnable,
                                                 VkBool32 stencilTestEnable,
                                                 VkCompareOp depthCompareOp)
  {
    depthStencilCi.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilCi.depthTestEnable = depthTestEnable;
    depthStencilCi.depthWriteEnable = depthWriteEnable;
    depthStencilCi.depthCompareOp = depthCompareOp;
    depthStencilCi.depthBoundsTestEnable = VK_FALSE;
    depthStencilCi.stencilTestEnable = VK_FALSE;
    //todo : check need the stencil setting
  }

  void VkPipelinePool::buildFragmentPipeline(VkPipelineShaderStageCreateInfo& shaderStateCi, VkShaderModule fragModule)
  {
    shaderStateCi.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStateCi.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    shaderStateCi.module = fragModule;
    shaderStateCi.pName = "main";
  }

  void VkPipelinePool::buildVertexPipeline(VkPipelineShaderStageCreateInfo& shaderStateCi, VkShaderModule vertexModule)
  {
    shaderStateCi.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStateCi.stage = VK_SHADER_STAGE_VERTEX_BIT;
    shaderStateCi.module = vertexModule;
    shaderStateCi.pName = "main";
  }

  void VkPipelinePool::buildAssemblyPipeline(VkPipelineInputAssemblyStateCreateInfo& inputAssembly,
                                             VkPrimitiveTopology topology,
                                             VkBool32 primitiveRestartEnable,
                                             VkPipelineInputAssemblyStateCreateFlags
                                             flags)
  {
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = topology;
    inputAssembly.primitiveRestartEnable = primitiveRestartEnable;
    //inputAssembly.flags                  = flags;
  }

  void VkPipelinePool::buildDynamicPipelineDyscriptor(VkPipelineDynamicStateCreateInfo& dynamicStateCi,
                                                      std::vector<VkDynamicState>& dynamicStates,
                                                      VkPipelineViewportStateCreateInfo& viewportStateCi,
                                                      uint32_t viewCount,
                                                      VkBool32 dynamicStencilTestEnable,
                                                      VkBool32 dynamicStateDepthCompare,
                                                      VkBool32 dynamicStateVetexStride,
                                                      VkPipelineDynamicStateCreateFlags flags)
  {
    ///todo:
    /// if need -> create View and Scissor with array
    /// update all and create Mulit view mode
    /// temp-> just single view and multi draw call

    viewportStateCi.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportStateCi.viewportCount = viewCount;
    viewportStateCi.scissorCount = viewCount;

    dynamicStates.push_back(VK_DYNAMIC_STATE_VIEWPORT);
    dynamicStates.push_back(VK_DYNAMIC_STATE_SCISSOR);
    dynamicStates.push_back(VK_DYNAMIC_STATE_DEPTH_TEST_ENABLE);
    dynamicStates.push_back(VK_DYNAMIC_STATE_POLYGON_MODE_EXT);

    if (dynamicStencilTestEnable) dynamicStates.push_back(VK_DYNAMIC_STATE_STENCIL_TEST_ENABLE);
    if (dynamicStateDepthCompare) dynamicStates.push_back(VK_DYNAMIC_STATE_DEPTH_COMPARE_OP);
    if (dynamicStateVetexStride) dynamicStates.push_back(VK_DYNAMIC_STATE_VERTEX_INPUT_BINDING_STRIDE);

    dynamicStateCi.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicStateCi.dynamicStateCount = dynamicStates.size();
    dynamicStateCi.pDynamicStates = dynamicStates.data();
  }

  void VkPipelinePool::buildRasterizationPipeline(VkPipelineRasterizationStateCreateInfo& rasterizaCi,
                                                  VkCullModeFlags cullMode,
                                                  VkPolygonMode mode,
                                                  VkFrontFace front,
                                                  VkPipelineRasterizationStateCreateFlags flags)
  {
    rasterizaCi.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizaCi.depthClampEnable = VK_FALSE;
    rasterizaCi.rasterizerDiscardEnable = VK_FALSE;
    rasterizaCi.lineWidth = 1.0f;
    rasterizaCi.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizaCi.cullMode = cullMode;
    rasterizaCi.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizaCi.depthBiasEnable = VK_FALSE;
    //rasterizaCi.flags                   = flags;
  }

  void VkPipelinePool::buildMultiSamplingPipeline(VkPipelineMultisampleStateCreateInfo& multiSamplingCi,
                                                  VkSampleCountFlagBits samples,
                                                  VkPipelineMultisampleStateCreateFlags flags)
  {
    multiSamplingCi.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multiSamplingCi.rasterizationSamples = samples;
    //multiSamplingCi.flags                = flags;
  }

  void VkPipelinePool::buildColorBlendingAttachment(VkPipelineColorBlendAttachmentState& colorBlendAttachment,
                                                    uint32_t flags,
                                                    VkBool32 blendEnable)
  {
    //this can be vector
    colorBlendAttachment.colorWriteMask = flags;
    colorBlendAttachment.blendEnable = VK_FALSE;
  }

  void VkPipelinePool::buildColorBlendingPipeline(VkPipelineColorBlendStateCreateInfo& colorBlendingCi,
                                                  VkPipelineColorBlendAttachmentState& attachment,
                                                  uint32_t attachmentCount)
  {
    colorBlendingCi.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendingCi.attachmentCount = attachmentCount;
    colorBlendingCi.pAttachments = &attachment;
  }

  void VkPipelinePool::buildDynamicRenderingPipeline(VkPipelineRenderingCreateInfo& dynamicRendering,
                                                     VkFormat* colorAttachmentFormats,
                                                     uint32_t colorAttachmentCount,
                                                     uint32_t viewMask,
                                                     VkFormat depthFormat,
                                                     VkFormat stencilAttachment)
  {
    dynamicRendering.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    dynamicRendering.pColorAttachmentFormats = colorAttachmentFormats;
    dynamicRendering.colorAttachmentCount = colorAttachmentCount;
    dynamicRendering.depthAttachmentFormat = VK_FORMAT_D32_SFLOAT;
    dynamicRendering.viewMask = viewMask;
    dynamicRendering.stencilAttachmentFormat = stencilAttachment;
  }

  VkPipelineLayout VkPipelinePool::createPipelineLayout(
    VkDescriptorSetLayout* descriptorLayoutData,
    uint32_t descriptorSetCount
    )
  {
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(gpu::VkContant);

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = descriptorSetCount;
    pipelineLayoutInfo.pSetLayouts = descriptorLayoutData;
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    if (vkCreatePipelineLayout(device_, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
    {
      throw std::runtime_error("devicecreate pipeline layout!");
    }
    return pipelineLayout;
  }

  VkPipeline VkPipelinePool::createPipeline(VertexType type,
                                            VkShaderModule vertexModule,
                                            VkShaderModule fragModule,
                                            RenderingAttachmentType renderingType,
                                            VkPipelineLayout pipelineLayout,
                                            uint32_t viewMask,
                                            VkFormat depthFormat,
                                            VkFormat stencilAttachment,

                                            VkPrimitiveTopology topology,
                                            VkCullModeFlags cullMode,
                                            VkBool32 depthTestEnable,
                                            VkBool32 depthWriteEnable,
                                            VkBool32 stencilTestEnable,
                                            VkCompareOp depthCompareOp)
  {
    VkPipelineShaderStageCreateInfo vertexInputInfo{};
    buildVertexPipeline(vertexInputInfo, vertexModule);

    VkVertexInputBindingDescription vertexInputBinding{};
    std::vector<VkVertexInputAttributeDescription> vertexInputAttribute{};
    VkPipelineVertexInputStateCreateInfo vertexStateInfo{};

    buildVertexDescriptor(type, vertexInputBinding, vertexInputAttribute, vertexStateInfo, 0);

    VkPipelineShaderStageCreateInfo fragShaderInputInfo{};
    buildFragmentPipeline(fragShaderInputInfo, fragModule);

    VkPipelineShaderStageCreateInfo shaderStage[] = {vertexInputInfo, fragShaderInputInfo};

    VkPipelineDepthStencilStateCreateInfo depthStencilState{};
    buildDepthStencilPipeline(depthStencilState, depthTestEnable, depthWriteEnable);

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState{};
    buildAssemblyPipeline(inputAssemblyState, topology, false);

    VkPipelineDynamicStateCreateInfo dynamicStateInfo{};
    std::vector<VkDynamicState> dynamicStates{};
    VkPipelineViewportStateCreateInfo viewportState{};

    buildDynamicPipelineDyscriptor(dynamicStateInfo,
                                   dynamicStates,
                                   viewportState);


    VkPipelineRasterizationStateCreateInfo rasterizerState{};
    buildRasterizationPipeline(rasterizerState, cullMode);

    VkPipelineMultisampleStateCreateInfo multisampleState{};
    buildMultiSamplingPipeline(multisampleState);

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    buildColorBlendingAttachment(colorBlendAttachment);

    VkPipelineColorBlendStateCreateInfo colorBlendState{};
    buildColorBlendingPipeline(colorBlendState, colorBlendAttachment);

    VkPipelineRenderingCreateInfo dRenderingInfo{};
    std::vector<VkFormat> formats = {
      VK_FORMAT_B8G8R8A8_SRGB
    };
    switch (renderingType)
    {
      case (RenderingAttachmentType::DEPTH):
      {
        buildDynamicRenderingPipeline(dRenderingInfo,
                                      nullptr,
                                      0);
        break;
      }
      case (RenderingAttachmentType::COLOR):
      {
        buildDynamicRenderingPipeline(dRenderingInfo,
                                      formats.data(),
                                      formats.size());
      }
    }
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStage;
    pipelineInfo.pVertexInputState = &vertexStateInfo;
    pipelineInfo.pInputAssemblyState = &inputAssemblyState;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizerState;
    pipelineInfo.pMultisampleState = &multisampleState;
    pipelineInfo.pColorBlendState = &colorBlendState;
    pipelineInfo.pDynamicState = &dynamicStateInfo;
    pipelineInfo.pDepthStencilState = &depthStencilState;
    pipelineInfo.pNext = &dRenderingInfo;
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.renderPass = VK_NULL_HANDLE;
    pipelineInfo.subpass = 0;

    VkPipeline pipeline = VK_NULL_HANDLE;
    VK_ASSERT(vkCreateGraphicsPipelines(device_,
                VK_NULL_HANDLE,
                1,
                &pipelineInfo,
                nullptr,
                &pipeline));

    VkPipelineProgram program;
    program.vertShaderModule = vertexModule;
    program.fragShaderModule = fragModule;
    program.topology = topology;
    pipelineHash_[program] = pipeline;

    return pipeline;
  }

  std::vector<uint8_t> VkPipelinePool::loadPipelineCache(
    const std::string& filename
    )
  {
    std::ifstream inFile(filename, std::ios::binary | std::ios::ate);
    if (!inFile.is_open())
    {
      spdlog::info("no pipeline cache data");
      return {};
    }
    std::streamsize size = inFile.tellg();
    inFile.seekg(0, std::ios::beg);
    std::vector<uint8_t> buffer(size);
    if (!inFile.read(reinterpret_cast<char*>(buffer.data()), size))
    {
      spdlog::info("failed to read pipeline cache");
      return {};
    }
    return buffer;
  }
}
