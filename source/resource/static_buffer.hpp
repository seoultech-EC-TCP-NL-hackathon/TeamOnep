#ifndef BUFFER_HPP
#define BUFFER_HPP
#include <common.hpp>
#include "vk_memory_allocator.hpp"
#include "IBuffer.hpp"
class StaticBuffer{
public:
  StaticBuffer(gpu::VkMemoryAllocator &allocator,
               VkDeviceSize bufferSize,
               BufferType type);

  ~StaticBuffer();
  void copyBuffer(VkCommandBuffer commandBuffer);
  void createMainBuffer();
  void createUniformBuffer();
  void loadDqata(const void *data, VkDeviceSize size);
  void getStagingBuffer(const void *data);
  const VkBuffer *getBuffer();
  const VkBuffer getStagingBuffer();

private:
  VkDevice device;
  VkPhysicalDevice physicalDevice;
  gpu::VkMemoryAllocator &allocator;
  VkBuffer stagingBuffer;
  VkBuffer buffer;
  gpu::VkAllocation stagingAllocation;
  gpu::VkAllocation allocation;
  VkDeviceSize bufferSize;
  BufferType type;
  bool stage(BufferType type);

  VkBufferUsageFlags getUsageFlags(BufferType type);
};
#endif //BUFFER_HPP