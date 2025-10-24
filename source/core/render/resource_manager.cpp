#include "resource_manager.hpp"
#include "../GPU/context.hpp"

void ResourceManager::setLight()
{
}

ResourceManager::ResourceManager(const ResourceManagerCreateInfo& info) :
  device_(info.device),
  allocator_(*info.allocator),
  maxInflight_(info.maxInflight)
{
  textures_.resize(1024);
  uiNeedTextures.reserve(1024);
  samplerBuilder_ = std::make_unique<SamplerBuilder>(device_);
  std::unique_ptr<Material> base = std::make_unique<Material>();
  base->name = "base";
  materials_[base->name] = std::move(base);
  auto lightBuf = gpu::VkGraphBuilder::buildHostBuffer(sizeof(lightUBO), BufferType::UNIFORM);
  lightBuilder.bufferContext = lightBuf;
  Light light;
  light.direction = glm::vec3(0, 0, 0);
  light.type = LightType::Point;
  light.intensity = 0.5;
  light.color = glm::vec3(0.3, 0.2, 0.5);
  Light light2;
  light.direction = glm::vec3(0, 0, 0);
  light.type = LightType::Point;
  light.intensity = 100;
  light.color = glm::vec3(0.3, 0.2, 0.5);
  Light light3;
  light.type = LightType::Point;
  light.direction = glm::vec3(0, 0, 0);
  light.intensity = 1000;
  light.color = glm::vec3(0.3, 0.2, 0.5);
  lightBuilder.build(light);
  lightBuilder.build(light2);
  lightBuilder.build(light3);
  lightBuilder.uploadData();
  currentCamBuffer.resize(info.maxInflight);

  for (int i = 0; i < gpu::ctx__->renderingContext.maxInflight__; i++)
  {
    currentCamBuffer[i] = gpu::VkGraphBuilder::buildHostBuffer(sizeof(CameraUBO),
                                                               BufferType::UNIFORM);
    currentCamBuffer[i].set = gpu::ctx__->pDescriptorAllocator->allocate(i);
    gpu::ctx__->pDescriptorAllocator->writeUbo(currentCamBuffer[i].bufferh_,
                                               currentCamBuffer[i].size_,
                                               currentCamBuffer[i].set,
                                               gpu::CAMERA_BINDING,
                                               0,
                                               1);
    lightBuilder.bufferContext.set = currentCamBuffer[i].set;

    //gpu::ctx__->pDescriptorAllocator->writeUbo(lightBuilder.bufferContext.bufferh_,
    //                                           lightBuilder.bufferContext.size_,
    //                                           lightBuilder.bufferContext.set,
    //                                           1,
    //                                           0,
    //                                           1);
    gpu::ctx__->pDescriptorAllocator->update();
  }
}

ResourceManager::~ResourceManager() = default;

void ResourceManager::updateMaincamState(uint32_t update)
{
  camera.update();
  currentCamBuffer[update].data_ = &(camera.ubo);
  currentCamBuffer[update].size_ = sizeof(camera.ubo);
  currentCamBuffer[update].uploadData();
}


void ResourceManager::uploadMesh(VkCommandBuffer command, std::string path)
{
  std::string modelPath = path;
  spdlog::info("Loading {} ", modelPath.c_str());
  Mesh tempMesh = importer_.loadModel(modelPath.c_str(), allocator_);
  tempMesh.name = path;
  std::unique_ptr<Mesh> mesh = std::make_unique<Mesh>(tempMesh.getVertices(), tempMesh.getIndices(), allocator_);

  mesh->copyBuffer(command);
  mesh->selected = true;
  if (selectedModel.mesh != nullptr)
  {
    selectedModel.mesh->selected = false;
    if (mesh->name == selectedModel.mesh->name)
    {
      mesh->name = selectedModel.mesh->name + "t";
    }
  }
  selectedModel.mesh = mesh.get();
  selectedModel.material = materials_["base"].get();
  meshes_[path] = std::move(mesh);
}

Mesh ResourceManager::uploadMesh(std::string path)
{
  std::string modelPath = path;
  spdlog::info("Loading {} ", modelPath.c_str());
  Mesh tempMesh = importer_.loadModel(modelPath.c_str(), allocator_);
  tempMesh.name = path;
  return std::move(tempMesh);
}

void ResourceManager::uploadTexture(VkCommandBuffer command, std::string path)
{
  TextureCreateInfo textureInfo;
  textureInfo.device = device_;
  textureInfo.sampler = samplerBuilder_->get();
  std::string texturePath = std::string(ASSET_TEXTURES_DIR) + path;
  textureInfo.filename = texturePath.c_str();
  textureInfo.allocator = &allocator_;
  auto texture = std::make_unique<VulkanTexture>(textureInfo);
  if (!texture->uploadedReady)
  {
    spdlog::info("fail to load image");
    return;
  }
  texture->bindigIndex = currentBinding_;
  texture->copyBufferToImage(command);
  //texture->descriptor = bindlessDescirptor_;
  //texture->uploadDescriptor(bindlessDescirptor_, currentBinding_);
  uiNeedTextures.push_back(texture.get());
  textures_[currentBinding_] = std::move(texture);
  currentBinding_++;
}
