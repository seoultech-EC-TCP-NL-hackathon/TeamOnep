#ifndef SHADERMOUDULE_HPP
#define SHADERMOUDULE_HPP

#include <string>
#include <filesystem>
#include <unordered_map>
#include <vulkan/vulkan.h>
#include <shaderc/shaderc.hpp>

//spirv reflect -> compile and register dsl
namespace gpu
{
  class VkShaderPool{
  public:
    VkShaderPool(VkDevice device);
    ~VkShaderPool();
    VkShaderModule getShader(std::string filePath, shaderc_shader_kind flag);

  private:
    void compileShader(const std::string& shaderSource, const std::string& outputFilePath, shaderc_shader_kind flag) const;

    void updateBinPath(std::string filepath);

  private:
    VkDevice device_;
    VkShaderModule shader;
    std::unordered_map<std::string, VkShaderModule> shaderModules_;
    std::string shaderPath_;
    std::string binPath_;
    shaderc::Compiler compiler;
    shaderc::CompileOptions options;
  };
}


#endif
