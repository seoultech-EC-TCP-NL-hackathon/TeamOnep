//
// Created by dlwog on 25. 10. 26..
//

#ifndef MYPROJECT_VTUBER_HPP
#define MYPROJECT_VTUBER_HPP
#include "CubismSdkForNative-5-r.4.1/Core/include/Live2DCubismCore.h"
#include "CubismSdkForNative-5-r.4.1/Framework/src/CubismFramework.hpp"
#include "CubismSdkForNative-5-r.4.1/Framework/src/Rendering/vk/CubismRenderer_Vulkan.hpp"
#include "CubismSdkForNative-5-r.4.1/Framework/src/Model/CubismModel.hpp"
#include "vulkan/vulkan.h"

using namespace Live2D::Cubism::Framework;
using namespace Live2D::Cubism::Framework::Rendering::Vulkan;

void InitCubism(VkDevice device, VkPhysicalDevice physicalDevice, VkRenderPass renderPass)
{
  CubismFramework::Option opt;
  opt.LogFunction = [](const char* msg) { printf("[Cubism] %s\n", msg); };
  opt.LoggingLevel = CubismFramework::Option::LogLevel_Verbose;
  CubismFramework::StartUp(nullptr, &opt);
  CubismFramework::Initialize();

  // 렌더러 초기화
  CubismRenderer_Vulkan::StaticInitialize(device, physicalDevice, renderPass);
}

std::unique_ptr<CubismUserModel> model;

void LoadModel(VkCommandBuffer cmd, const std::string& path)
{
  model = std::make_unique<CubismUserModel>();
  model->LoadAssets(path.c_str(), "model3.json"); // path 내 model3.json 기준
  model->CreateRenderer();
  auto renderer = static_cast<CubismRenderer_Vulkan*>(model->GetRenderer());
  renderer->Initialize(cmd);
}

void RenderFrame(VkCommandBuffer cmd)
{
  model->Update(); // Physics, motion 등 업데이트
  auto renderer = static_cast<CubismRenderer_Vulkan*>(model->GetRenderer());
  renderer->BindTexture(0, textureId); // 텍스처 바인딩 (샘플 구조 참고)
  renderer->DrawModel(cmd);
  model->StartMotion("Idle", 0, 1);    // Idle 모션 시작
  model->SetExpression("Smile");       // 표정 변경
  model->GetModel()->SetParameterValueById("ParamAngleX", x);  // 수동 제어
}


#endif //MYPROJECT_VTUBER_HPP
