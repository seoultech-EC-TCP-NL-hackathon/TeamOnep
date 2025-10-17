#ifndef MYPROJECT_DESCRIPTOR_LAYOUT_BUILDER_HPP
#define MYPROJECT_DESCRIPTOR_LAYOUT_BUILDER_HPP
#ifndef MYPROJECT_DESCRIPTOR_SET_LAYOUT_HPP
#define MYPROJECT_DESCRIPTOR_SET_LAYOUT_HPP
#include<vulkan/vulkan.h>
#include<unordered_map>
#include<vector>
#include<string>
#include "node.hpp"
#include <descriptor_layout_config.hpp>

struct LayoutBindingInfo{
  gpu::descriptor_usage usage;
  VkShaderStageFlags stage;
  uint32_t bindingIndex;
};

///todo:
/// shader stage setting
class DescriptorLayoutBuilder{
public:
  DescriptorLayoutBuilder(VkDevice device);
  ~DescriptorLayoutBuilder();
  VkDescriptorSetLayout createDescriptorSetLayout(std::vector<LayoutBindingInfo> &infos);
  void buildUboLayout(std::vector<VkDescriptorSetLayoutBinding> &bindings,
                      uint32_t bindingIndex,
                      VkShaderStageFlags stage,
                      std::vector<VkDescriptorBindingFlags> &bindingFlags,
                      uint32_t arrayCnt = 1);

  void buildBindlessTextureLayout(std::vector<VkDescriptorSetLayoutBinding> &bindings,
                                  uint32_t bindingIndex,
                                  VkShaderStageFlags stage,
                                  std::vector<VkDescriptorBindingFlags> &bindingFlags,
                                  uint32_t arrayCnt = gpu::BINDELSS_TEXTURE_ARRAY_COUNT);

  void buildDynamicUboLayout(std::vector<VkDescriptorSetLayoutBinding> &bindings,
                             uint32_t bindingIndex,
                             VkShaderStageFlags stage,
                             std::vector<VkDescriptorBindingFlags> &bindingFlags,
                             uint32_t arrayCnt = gpu::DYNAMIC_UBO_ARRAY_COUNT);

  void buildSsboLayout(std::vector<VkDescriptorSetLayoutBinding> &bindings,
                       uint32_t bindingIndex,
                       VkShaderStageFlags stage,
                       std::vector<VkDescriptorBindingFlags> &bindingFlags,
                       uint32_t arrayCnt = 1);

  void buildTextureLayout(std::vector<VkDescriptorSetLayoutBinding> &bindings,
                          uint32_t bindingIndex,
                          VkShaderStageFlags stage,
                          std::vector<VkDescriptorBindingFlags> &bindingFlags,
                          uint32_t arrayCnt);

private:
  VkDevice device_;
};

#endif //MYPROJECT_DESCRIPTOR_SET_LAYOUT_HPP
#endif //MYPROJECT_DESCRIPTOR_LAYOUT_BUILDER_HPP