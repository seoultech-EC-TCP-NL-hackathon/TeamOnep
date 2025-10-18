#include "vk_graph.hpp"
#include "vk_command_buffer.hpp"

gpu::VkGraph::VkGraph(VkContext* context)
  : pCtxt_(context)
{
}

gpu::VkGraph::~VkGraph() = default;

void gpu::VkGraph::execute(VkCommandBuffer cmd)
{
  for (auto& pass : pCtxt_->compiledPass)
  {
    if (!pass.culled)
    {
      pass.execute(cmd);
      if (pass.transitionPass)
      {
        pass.culled = true;
      }
    }
  }
  return;
}
