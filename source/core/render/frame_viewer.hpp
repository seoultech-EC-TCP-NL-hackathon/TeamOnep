#ifndef MYPROJECT_FRAME_VIEWER_HPP
#define MYPROJECT_FRAME_VIEWER_HPP
#include <vector>
#include <cmath>
#include <algorithm>
#include "imnodes.h"
#include "context.hpp"

// Force-directed layout parameters
const float REPULSION = 5000.0f;
const float ATTRACTION = 0.1f;
const float DAMPING = 0.85f;
class RenderPassBuilder;
struct RenderTarget
{
  int id;
  const char* name;
  int input_attr_id;
  int output_attr_id;
  gpu::Pipeline drawHandle;
  ImVec2 pos = ImVec2(0, 0);
  ImVec2 size = ImVec2(200, 200);
};

struct Link
{
  int id;
  int start_attr_id;
  int end_attr_id;
};

class FrameDebug
{
  public:
  FrameDebug();
  void show();
  void UpdateForceDirected();
  std::vector<RenderTarget> nodes;
  std::vector<Link> links;
  int next_node_id = 1;
  int next_link_id = 1;
  RenderPassBuilder* passBuilder;
};


#endif //MYPROJECT_FRAME_VIEWER_HPP
