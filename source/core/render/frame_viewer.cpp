#include "frame_viewer.hpp"
#include "context.hpp"
#include "renderpass_builder.hpp"

FrameDebug::FrameDebug()
{
  auto ctx = ImNodes::CreateContext();
  ImNodes::SetCurrentContext(ctx);
  ImNodes::StyleColorsDark();
}

void FrameDebug::show()
{
  ImGui::Begin("Force-Directed Graph");
  if (ImGui::Button("Add Node"))
  {
    RenderTarget node;
    node.id = next_node_id++;
    node.name = "Node";
    node.input_attr_id = node.id * 2 - 1;
    node.output_attr_id = node.id * 2;

    node.pos = ImVec2(rand() % 400, rand() % 400);
    node.size = ImVec2(200, 200);
    nodes.push_back(node);

    // 이전 노드와 자동 연결
    if (nodes.size() > 1)
    {
      Link link;
      link.id = next_link_id++;
      link.start_attr_id = nodes[nodes.size() - 2].output_attr_id;
      link.end_attr_id = node.input_attr_id;
      links.push_back(link);
    }
  }
  ImNodes::BeginNodeEditor();

  for (auto& node : nodes)
  {
    ImNodes::BeginNode(node.id);

    ImNodes::BeginNodeTitleBar();
    ImGui::Text("%s %d", node.name, node.id);
    ImNodes::EndNodeTitleBar();

    ImNodes::BeginInputAttribute(node.input_attr_id);
    ImGui::Text("Input");
    ImNodes::EndInputAttribute();

    ImNodes::BeginOutputAttribute(node.output_attr_id);
    ImGui::Text("Output");
    ImNodes::EndOutputAttribute();
    ImGui::BeginChild(("node_content_" + std::to_string(node.id)).c_str(),
                      node.size);
    ImVec2 pos = ImGui::GetWindowPos();
    ImVec2 size = ImGui::GetWindowSize();
    ImGui::Text("Node content");
    ImGui::Text("Node %f %f", node.pos.x, pos.y );

    node.pos = pos;
    node.size = size;
    ImGui::EndChild();
    ImNodes::EndNode();
  }
  if (nodes.size() > 0)
  {
    passBuilder->nodes = nodes;
  }

  for (auto& link : links)
  {
    ImNodes::Link(link.id,
                  link.start_attr_id,
                  link.end_attr_id
                 );
  }
  //int start_attr, end_attr;
  //if (ImNodes::IsLinkCreated(&start_attr, &end_attr))
  //{
  //  Link link;
  //  link.id = next_link_id++;
  //  link.start_attr_id = start_attr;
  //  link.end_attr_id = end_attr;
  //  links.push_back(link);
  //}

  //int destroyed_link_id;
  //if (ImNodes::IsLinkDestroyed(&destroyed_link_id))
  //{
  //  links.erase(
  //              std::remove_if(links.begin(), links.end(),
  //                             [destroyed_link_id](const Link& l) { return l.id == destroyed_link_id; }),
  //              links.end());
  //}
  ImNodes::EndNodeEditor();
  ImGui::End();
}
