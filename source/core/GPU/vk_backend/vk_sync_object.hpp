#ifndef MYPROJECT_RENDER_GRAPH_SYNC_HPP
#define MYPROJECT_RENDER_GRAPH_SYNC_HPP

#include "vk_common.hpp"

namespace gpu
{
  class VkContext;

  struct VkSemaphorePool{
  public:
    VkSemaphorePool(VkContext* pCtxt);
    ~VkSemaphorePool();
    void recreate();
    VkContext* pCtxt;
    uint32_t MAX_FRAMES_IN_FLIGHT__;
    std::vector<VkSemaphore> semaphores__;
  };

  struct VkFencePool{
    VkFencePool(VkContext* pCtxt, bool signaled = true);

    ~VkFencePool();

    void recreate();

    VkContext* pCtxt;
    uint32_t MAX_FRAMES_IN_FLIGHT__;
    std::vector<VkFence> fences;
  };

  struct Query{
    VkQueryPool queryPool = VK_NULL_HANDLE;
    uint32_t queryCount   = 0;
    // query type: timestamp, occlusion, pipeline statistics
    VkQueryType type;

    // 생성
    void create(VkDevice device, VkQueryType queryType, uint32_t count)
    {
      type       = queryType;
      queryCount = count;

      VkQueryPoolCreateInfo info{};
      info.sType      = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
      info.queryType  = queryType;
      info.queryCount = count;

      vkCreateQueryPool(device, &info, nullptr, &queryPool);
      //vkCmdBeginQuery()
    }

    void destroy(VkDevice device)
    {
      if (queryPool != VK_NULL_HANDLE)
      {
        vkDestroyQueryPool(device, queryPool, nullptr);
        queryPool = VK_NULL_HANDLE;
      }
    }

    void
    getResults(VkDevice device, uint32_t firstQuery, uint32_t queryCount, void* data, VkDeviceSize stride,
               VkQueryResultFlags flags)
    {
      vkGetQueryPoolResults(device, queryPool, firstQuery, queryCount, stride, data, stride, flags);
    }
  };
}


#endif //MYPROJECT_RENDER_GRAPH_SYNC_HPP
