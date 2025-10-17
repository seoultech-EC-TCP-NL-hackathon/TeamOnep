//
// Created by ljh on 25. 9. 19..
//

#ifndef MYPROJECT_DYN_BUFFER_HPP
#define MYPROJECT_DYN_BUFFER_HPP
#include "static_buffer.hpp"
#include "../../core/scene_graph/mesh.hpp"
#include <unordered_set>
#include "brush.hpp"

struct DynaMesh{
  DynaMesh(Mesh *mesh);
  ~DynaMesh();
  void reAllocate();
  void dynMeshUpdate(VkCommandBuffer commandBuffer);
  void subdivideTriange(uint32_t triangleIndex);
  uint64_t sculptMidPoint(uint32_t i0, uint32_t i1);
  uint64_t getVertexCash(uint32_t a, uint32_t b);
  void subdivideMesh();
  void offsetCheck();
  void updateAdj()
  {
    std::vector<VertexAll> &vertices         = mesh_->vertices;
    const std::vector<unsigned int> &indices = mesh_->indices;
    float lambda                             = 0.5;
    int iterations                           = 1;
    adjacency.resize(vertices.size());
    size_t nVerts = vertices.size();
    for (size_t i = 0; i < indices.size(); i += 3)
    {
      unsigned int a = indices[i];
      unsigned int b = indices[i + 1];
      unsigned int c = indices[i + 2];

      adjacency[a].insert(b);
      adjacency[a].insert(c);

      adjacency[b].insert(a);
      adjacency[b].insert(c);

      adjacency[c].insert(a);
      adjacency[c].insert(b);
    }
  }
  void smoothVertices()
  {
    std::vector<VertexAll> &vertices         = mesh_->vertices;
    const std::vector<unsigned int> &indices = mesh_->indices;
    float lambda                             = 0.5;
    int iterations                           = 1;
    adjacency.resize(vertices.size());
    size_t nVerts = vertices.size();
    for (size_t i = 0; i < indices.size(); i += 3)
    {
      unsigned int a = indices[i];
      unsigned int b = indices[i + 1];
      unsigned int c = indices[i + 2];

      adjacency[a].insert(b);
      adjacency[a].insert(c);

      adjacency[b].insert(a);
      adjacency[b].insert(c);

      adjacency[c].insert(a);
      adjacency[c].insert(b);
    }
    std::vector<VertexAll> newVertices = vertices;
    for (int iter = 0; iter < iterations; ++iter)
    {
      for (size_t i = 0; i < nVerts; ++i)
      {
        glm::vec3 sumNeighbors(0.0f);
        for (auto j: adjacency[i])
          sumNeighbors += vertices[j].position;
        if (!adjacency[i].empty())
        {
          glm::vec3 avg  = sumNeighbors / static_cast<float>(adjacency[i].size());
          newVertices[i].position = vertices[i].position + lambda * (avg - vertices[i].position);
        }
      }
      vertices = newVertices; // 업데이트
    }
  }

  Mesh *mesh_;
  bool dirty_ = false;
  std::unordered_map<uint64_t, uint32_t> midpointCache;
  std::vector<std::unordered_set<unsigned int> > adjacency;
  VkDeviceSize vertexOffset;
  VkDeviceSize indexOffset;
  VkDeviceSize vertexSize_;
  VkDeviceSize indexSize_;
  gpu::VkMemoryAllocator *pAllocator;
};

#endif //MYPROJECT_DYN_BUFFER_HPP