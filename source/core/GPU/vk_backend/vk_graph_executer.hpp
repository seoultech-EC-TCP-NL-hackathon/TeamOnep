#ifndef VK_RENDER_GRAPH__
#define VK_RENDER_GRAPH__

#include "vk_common.hpp"
#include "vk_swapchain.hpp"
#include "vk_command_buffer.hpp"
#include "vk_graph_compiler.hpp"
#include "vk_sync_object.hpp"
#include "vk_node.hpp"

namespace gpu
{
  class VkGraph{
  public:
    VkGraph(gpu::VkContext* pCtxt);
    ~VkGraph();
    void execute(VkCommandBuffer cmd);
  private:
    gpu::VkContext* pCtxt_;
  };
}

#endif
