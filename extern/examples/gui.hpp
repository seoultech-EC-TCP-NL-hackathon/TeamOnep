//
// Created by ljh on 25. 9. 22..
//

#ifndef MYPROJECT_GUI_HPP
#define MYPROJECT_GUI_HPP
{
static char buf[256] = "";
ImGui::InputTextWithHint("Mesh Load", "Mesh/Load/Path....", buf, IM_ARRAYSIZE(buf));
static int selected_hint = -1;

if (ImGui::Button("load Mesh"))
{
  UICall call;
  call.path = buf;
  call.type = CallType::Mesh;
  callStack_.push_back(call);
  spdlog::info("mesh call {}", call.path);
  buf[0] = '\0';
}
}
{
static char buf[256] = "";
ImGui::InputTextWithHint("Texture path:", "texture/Load/Path....", buf, IM_ARRAYSIZE(buf));

if (ImGui::Button("load Texture"))
{
  UICall call;
  call.path = buf;
  call.type = CallType::Texture;
  callStack_.push_back(call);
  spdlog::info("mesh call {}", call.path);
  buf[0] = '\0';
}
}
#endif //MYPROJECT_GUI_HPP