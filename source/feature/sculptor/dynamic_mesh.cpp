//
// Created by ljh on 25. 9. 19..
//

#include "dynamic_mesh.hpp"

DynaMesh::DynaMesh(Mesh *mesh) :
  mesh_(mesh),
  pAllocator(&mesh->allocator)
{
  reAllocate();
}

DynaMesh::~DynaMesh() = default;

void DynaMesh::reAllocate()
{
  mesh_->vertexBuffer.reset();
  mesh_->indexBuffer.reset();
  vertexOffset = (sizeof(VertexAll)) * mesh_->vertices.size();
  indexOffset  = sizeof(4) * mesh_->indices.size();
  mesh_->vertexSize *= 2;
  mesh_->indiceSize *= 2;
  vertexSize_         = mesh_->vertexSize;
  indexSize_          = mesh_->indiceSize;
  mesh_->vertexBuffer = std::make_unique<StaticBuffer>(*pAllocator,
                                                       mesh_->vertexSize,
                                                       BufferType::VERTEX);

  mesh_->indexBuffer = std::make_unique<StaticBuffer>(*pAllocator,
                                                      mesh_->indiceSize,
                                                      BufferType::INDEX);

  mesh_->vertexBuffer->getStagingBuffer(mesh_->vertices.data());
  mesh_->indexBuffer->getStagingBuffer(mesh_->indices.data());
  mesh_->vertexBuffer->createMainBuffer();
  mesh_->indexBuffer->createMainBuffer();
  spdlog::info(" mesh re allocate");
  dirty_ =true;

}

void DynaMesh::dynMeshUpdate(VkCommandBuffer commandBuffer)
{
  mesh_->vertexBuffer->getStagingBuffer(mesh_->vertices.data());
  mesh_->indexBuffer->getStagingBuffer(mesh_->indices.data());
  mesh_->vertexBuffer->copyBuffer(commandBuffer);
  mesh_->indexBuffer->copyBuffer(commandBuffer);
  dirty_ =false;
}

void DynaMesh::subdivideTriange(uint32_t triangleIndex)
{
  std::vector<uint32_t> newIndices;
  uint32_t i0 = mesh_->indices[3 * triangleIndex];
  uint32_t i1 = mesh_->indices[3 * triangleIndex + 1];
  uint32_t i2 = mesh_->indices[3 * triangleIndex + 2];

  uint32_t m0 = sculptMidPoint(i0, i1);
  uint32_t m1 = sculptMidPoint(i1, i2);
  uint32_t m2 = sculptMidPoint(i2, i0);

  newIndices.insert(newIndices.end(), {i0, m0, m2});
  newIndices.insert(newIndices.end(), {i1, m1, m0});
  newIndices.insert(newIndices.end(), {i2, m2, m1});
  newIndices.insert(newIndices.end(), {m0, m1, m2});

  mesh_->indices.insert(mesh_->indices.end(),
                        newIndices.begin(),
                        newIndices.end());
  offsetCheck();
}

uint64_t DynaMesh::sculptMidPoint(uint32_t i0, uint32_t i1)
{
  uint64_t key = getVertexCash(i0, i1);
  if (midpointCache.count(key))
  {
    return midpointCache[key];
  }
  glm::vec3 p0 = mesh_->vertices[i0].position;
  glm::vec3 p1 = mesh_->vertices[i1].position;
  glm::vec3 n0 = mesh_->vertices[i0].normal;
  glm::vec3 n1 = mesh_->vertices[i1].normal;
  VertexAll mid;
  mid.position = 0.5f * (p0 + p1);
  mid.normal   = glm::normalize(n0 + n1);

  uint32_t newIndex = static_cast<uint32_t>(mesh_->vertices.size());
  mesh_->vertices.push_back(mid);
  midpointCache[key] = newIndex;
  return newIndex;
}

uint64_t DynaMesh::getVertexCash(uint32_t a, uint32_t b)
{
  if (a > b) std::swap(a, b);
  return (static_cast<uint64_t>(a) << 32) | b;
}

void DynaMesh::subdivideMesh()
{
  std::vector<uint32_t> newIndices;
  for (size_t i = 0; i < mesh_->indices.size(); i += 3)
  {
    uint32_t i0 = mesh_->indices[i];
    uint32_t i1 = mesh_->indices[i + 1];
    uint32_t i2 = mesh_->indices[i + 2];

    uint32_t m0 = sculptMidPoint(i0, i1);
    uint32_t m1 = sculptMidPoint(i1, i2);
    uint32_t m2 = sculptMidPoint(i2, i0);

    newIndices.insert(newIndices.end(), {i0, m0, m2});
    newIndices.insert(newIndices.end(), {i1, m1, m0});
    newIndices.insert(newIndices.end(), {i2, m2, m1});
    newIndices.insert(newIndices.end(), {m0, m1, m2});
  }
  mesh_->indices = std::move(newIndices);
  spdlog::info("subdivided");
  offsetCheck();
}

void DynaMesh::offsetCheck()
{
  vertexOffset = (sizeof(VertexAll)) * mesh_->vertices.size();
  indexOffset  = sizeof(4) * mesh_->indices.size();
  if (vertexOffset > vertexSize_ | indexOffset > indexSize_)
  {
    reAllocate();
    spdlog::info("reallocated");
  }
}