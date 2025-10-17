#ifndef MYPROJECT_RESOURCE_TRACKER_HPP
#define MYPROJECT_RESOURCE_TRACKER_HPP

#include <memory>
#include <unordered_map>
#include <queue>
#include "vk_node.hpp"
#include "vk_memory_pool.hpp"
//#include "vk_pipeline_pool.hpp"
//#include "vk_shader_pool.hpp"
//#include "vk_sampler_builder.hpp"
//#include "vk_descriptor_uploader.hpp"

namespace gpu
{
  class VkContext;
  class VkDiscardPool{
  public:
    VkDiscardPool(VkContext* pCtxt);
    ~VkDiscardPool() = default;
    VkContext* pCtxt_;
    void clean();
    void registerResource(std::vector<VkNode*>& frameResource);
    std::function<void()> buildDiscardRec();

    struct DiscardHandle{
      VkNode* handler;
      std::function<void()> deleteFunction;
    };
    std::unordered_map<VkNodeId, DiscardHandle> handleHash_;
    std::vector<std::vector<DiscardHandle>> frameDiscardPool_;
  };
}


#endif //MYPROJECT_RESOURCE_TRACKER_HPP
