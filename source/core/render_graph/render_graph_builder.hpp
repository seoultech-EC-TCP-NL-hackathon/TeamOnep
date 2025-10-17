//
// Created by ljh on 25. 10. 13..
//

#ifndef MYPROJECT_RENDERING_GRAPH_HPP
#define MYPROJECT_RENDERING_GRAPH_HPP
#include "../GPU/context.hpp"
#include "unique.hpp"

namespace mns{
  class RenderGraphBuilder{
  public:
    void sort();
    void merge();
    void buildUiPass();
  };
};


//if (read->usage_ == GPU::ResourceUsage::MESH_BUFFER |
//    read->usage_ == GPU::ResourceUsage::VERTEX_BUFFER)
//{
//  VkBatch *mesh = reinterpret_cast<VkBatch *>(read);
//  pCtxt->batch.batches.push_back(mesh);
//}

#endif //MYPROJECT_RENDERING_GRAPH_HPP