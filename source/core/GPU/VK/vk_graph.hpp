#ifndef VK_RENDER_GRAPH__
#define VK_RENDER_GRAPH__
#include "vk_command_buffer.hpp"

namespace gpu
{
  class VkContext;
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
