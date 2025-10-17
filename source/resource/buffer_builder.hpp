//
// Created by ljh on 25. 9. 16..
//

#ifndef MYPROJECT_BUFFER_BUILDER_HPP
#define MYPROJECT_BUFFER_BUILDER_HPP

#include <vulkan/vulkan.h>
#include <unordered_map>
#include "static_buffer.hpp"
#include "vk_memory_allocator.hpp"

struct BufferContext{
  StaticBuffer* buffer;
  VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
  uint32_t bindingIndex         = 0;
};

class BufferBuilder;
using UBOBuilder          = BufferBuilder;
using SSBOBuilder         = BufferBuilder;
using VertexBufferBuilder = BufferBuilder;
using IndexBufferBuilder  = BufferBuilder;

using GlobalLightBuffer = BufferContext;

enum class AccessPolicy{
  Auto,
  HostPreferred,
  DeviceLocal
};

class BufferBuilder{
public:
  BufferBuilder(
      gpu::VkMemoryAllocator &allocator,
      BufferType type,
      AccessPolicy policy = AccessPolicy::Auto
    );
  ~BufferBuilder();
  BufferContext buildBuffer(VkDeviceSize size, const char *debugName = "Buffer");
  void destroy(VkBuffer Buffer);
  void upload(const void *data, VkDeviceSize size, VkDeviceSize dstOffset = 0);
  BufferType type() const;
  void map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

private:
  VkBufferUsageFlags usageFromType(BufferType t) const;
  void flush(VkDeviceSize offset, VkDeviceSize size);
  void unmap();
  void decidePolicy();

private:
  VkDevice device_;
  gpu::VkMemoryAllocator &allocator_;
  BufferType type_;
  AccessPolicy policy_;
  std::unordered_map<VkBuffer, std::unique_ptr< StaticBuffer>> allocatedBuffers;
};
#endif //MYPROJECT_BUFFER_BUILDER_HPP