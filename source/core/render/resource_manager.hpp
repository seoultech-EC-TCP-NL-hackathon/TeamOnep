#ifndef RESOURCE_MANAGER
#define RESOURCE_MANAGER
#include <common.hpp>
#include <../resource/importer.hpp>
#include <unordered_map>
#include "../scene_graph/material.hpp"
#include "push_constant.hpp"
#include "../scene_graph/model.hpp"
#include "vk_memory_allocator.hpp"
#include "vk_host_buffer.h"
#include "../scene_graph/camera_state.hpp"
#include "../GPU/texture.hpp"
#include "../scene_graph/mesh.hpp"
#include "../scene_graph/light.hpp"

using Key = std::string;

struct ResourceManagerCreateInfo{
  VkDevice device            = VK_NULL_HANDLE;
  gpu::VkMemoryAllocator *allocator = nullptr;
  uint32_t maxInflight       = 3;
};

using bindingIndex = uint32_t;

class ResourceManager{
  friend class Engine;
  friend class IRenderer;
  friend class UI;
public:
  gpu::DescriptorSetLayout layout;
  ResourceManager(const ResourceManagerCreateInfo &info);
  ~ResourceManager();
  void updateMaincamState(uint32_t currentFrame);
  void uploadMesh(VkCommandBuffer command, std::string path);
  Mesh uploadMesh(std::string path);
  void uploadTexture(VkCommandBuffer command, std::string path);
  void setLight();
  LightBuilder lightBuilder;
  std::vector<gpu::VkHostBuffer> currentCamBuffer;
  std::vector<VulkanTexture *> uploadedTexures;
  Model selectedModel{};
  ImporterEx importer_;
  uint32_t maxInflight_ = 3;
  bindingIndex currentBinding_ = 0;
private:
  UserCamera camera;
  bool shouldUpdate;
  VkDevice device_;
  gpu::VkMemoryAllocator &allocator_;
  std::vector<VulkanTexture *> uiNeedTextures;
  std::unordered_map<Key, std::unique_ptr<Mesh> > meshes_;
  std::unordered_map<Key, std::unique_ptr<Model> > models_;
  std::unordered_map<Key, std::unique_ptr<Material> > materials_;
  std::vector <std::unique_ptr<VulkanTexture> > textures_;
  std::unique_ptr<SamplerBuilder> samplerBuilder_;
  std::shared_ptr<VulkanTexture> nomal;

};

#endif