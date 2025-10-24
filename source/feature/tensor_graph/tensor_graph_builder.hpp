//
// Created by ljh on 25. 10. 13..
//

#ifndef MYPROJECT_COMPUTING_GRAPH_HPP
#define MYPROJECT_COMPUTING_GRAPH_HPP
#include <memory>
#include <functional>
#include <vector>

enum class Operation
{
  MUL,
  ADD,
};

class Node
{
  public:
  Operation op;
  double value = 0.0;
  double grad = 0.0;
  std::vector<Node*> read__;
  std::function<void()> backward;
};

// Backprop from this variable (assume scalar output)
///propagation function:
/// for writers,
///  method to add node depenency
class LayerDescriptor
{
  public:
  std::vector<LayerDescriptor*> read__;
  std::vector<LayerDescriptor*> write__;
  std::vector<Node*> nodes;
  int dependencyCount = 0;

  private:
  virtual void forwardPropagation() = 0;
  virtual void backwardPropagation() = 0;
  virtual void inferenceShape() = 0;
};


class ReLU : LayerDescriptor
{
  void forwardPropagation() override
  {
    //output = max(0, input);
  }

  void backwardPropagation() override
  {
    //grad_input = grad_output * (input > 0);
  }
};

class TensorGraphBuilder
{
  void addLayer();

  std::vector<LayerDescriptor*> layer;
  std::vector<Node*> nodes_;
  uint32_t layerId = 0;
};

void backward()
{
  // last = node->grad = 1.0; // d(out)/d(out)
  // traverse in reverse topological order
  //for (auto it = order.rbegin(); it != order.rend(); ++it)
  //{
  //  auto n = *it;
  //  if (n->backward) n->backward();
  //}
}


#endif //MYPROJECT_COMPUTING_GRAPH_HPP
