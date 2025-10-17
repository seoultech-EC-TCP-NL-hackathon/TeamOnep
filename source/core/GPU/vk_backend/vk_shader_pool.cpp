#include <spdlog/spdlog.h>
#include <vulkan/vulkan.h>
#include <stdexcept>
#include <fstream>
#include <vector>
#include "vk_shader_pool.hpp"
#include "vk_common.hpp"

namespace gpu
{
  VkShaderPool::VkShaderPool(VkDevice device)
    : device_(device),
      shader(VK_NULL_HANDLE)
  {
  }

  VkShaderPool::~VkShaderPool()
  {
    if (shader != VK_NULL_HANDLE)
    {
      vkDestroyShaderModule(device_, shader, nullptr);
    }
  }


  VkShaderModule VkShaderPool::getShader(std::string filepath,
                                         shaderc_shader_kind flag)
  {
    //shader path
    shaderPath_ = filepath;
    updateBinPath(filepath);

    std::ifstream file(binPath_.c_str(), std::ios::ate | std::ios::binary);
    spdlog::info("shader path: {}", filepath);

    compileShader(shaderPath_, binPath_, flag);
    spdlog::info("shader path: {}", filepath);
    std::ifstream binFile(binPath_, std::ios::ate | std::ios::binary);
    if (!binFile.is_open()) throw std::runtime_error("failed to open shader file");
    size_t fileSize = (size_t)binFile.tellg();
    std::vector<char> shaderBuffer(fileSize);
    binFile.seekg(0);
    binFile.read(shaderBuffer.data(), static_cast<long>(fileSize));
    binFile.close();
    VkShaderModuleCreateInfo ci{};
    ci.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    ci.codeSize = shaderBuffer.size();
    ci.pCode    = reinterpret_cast<const uint32_t*>(shaderBuffer.data());
    VK_ASSERT(vkCreateShaderModule(device_, &ci, nullptr, &shader));
    return shader;
  }

  void VkShaderPool::compileShader(const std::string& shaderSource,
                                   const std::string& outputFilePath,
                                   shaderc_shader_kind flag) const
  {
    std::ifstream glslCode(shaderSource.c_str(), std::ios::ate | std::ios::binary);
    spdlog::info("shader path: {}", shaderSource.c_str());
    size_t fileSize = (size_t)glslCode.tellg();
    std::vector<char> shaderBuffer(fileSize);
    glslCode.seekg(0);
    glslCode.read(shaderBuffer.data(), fileSize);
    glslCode.close();
    std::string shaderCode(shaderBuffer.begin(), shaderBuffer.end());
    shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(shaderCode,
                                                                     flag,
                                                                     shaderPath_.c_str()
    );

    if (result.GetCompilationStatus() != shaderc_compilation_status_success)
    {
      std::string errorMessage = result.GetErrorMessage();
      spdlog::error("Shader compilation failed: {}", errorMessage);
      throw std::runtime_error("Shader compilation failed.");
    }

    std::ofstream spvCode(outputFilePath, std::ios::binary);
    spvCode.write(
      reinterpret_cast<const char*>(result.cbegin()),
      (result.cend() - result.cbegin()) * sizeof(uint32_t)
    );
    spvCode.close();
    spdlog::info("compiled shader: {}", outputFilePath);
  }

  void VkShaderPool::updateBinPath(std::string filepath)
  {
    binPath_ = filepath + ".spv";
  }
}


//cashing
//}    //if (!file.is_open())
//{ cashing option
//size_t fileSize = (size_t) file.tellg();
//std::vector<char> shaderBuffer(fileSize);
//file.seekg(0);
//file.read(shaderBuffer.data(), fileSize);
//file.close();
//VkShaderModuleCreateInfo createInfo{};
//createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
//createInfo.codeSize = shaderBuffer.size();
//createInfo.pCode = reinterpret_cast<const uint32_t *>(shaderBuffer.data());
//VK_ASSERT(vkCreateShaderModule(device_, &createInfo, nullptr, &baseShader_));
