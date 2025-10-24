//
// Created by dlwog on 25. 10. 24..
//

#ifndef MYPROJECT_DLSS_HPP
#define MYPROJECT_DLSS_HPP
//#include "nvsdk_ngx.h"
//#include "nvsdk_ngx_helpers_vk.h"
//
//NVSDK_NGX_FeatureCommonInfo commonInfo = {};
//NVSDK_NGX_Result result;
//
//result = NVSDK_NGX_VULKAN_Init(
//    NVSDK_NGX_VULKAN_ID,          // Engine ID
//    L"AppName",                   // App 이름
//    device,                       // VkDevice
//    physicalDevice,                // VkPhysicalDevice
//    instance,                      // VkInstance
//    appVersion,                    // 임의 버전
//    nullptr, nullptr,              // Path 옵션
//    0
//);
//NVSDK_NGX_DLSS_Create_Params dlssParams = {};
//dlssParams.Feature.InWidth  = renderWidth;   // internal resolution
//dlssParams.Feature.InHeight = renderHeight;
//dlssParams.Feature.OutWidth = displayWidth;  // output resolution
//dlssParams.Feature.OutHeight = displayHeight;
//NVSDK_NGX_Parameter* params = nullptr;
//NVSDK_NGX_VULKAN_GetParameters(&params);
//
//NVSDK_NGX_VULKAN_SetTexture(params, NVSDK_NGX_Parameter_SuperSampling_Input_Color, colorImage);
//NVSDK_NGX_VULKAN_SetTexture(params, NVSDK_NGX_Parameter_SuperSampling_Input_Depth, depthImage);
//NVSDK_NGX_VULKAN_SetTexture(params, NVSDK_NGX_Parameter_SuperSampling_Input_MotionVectors, motionImage);
//NVSDK_NGX_VULKAN_ReleaseFeature(dlssFeature);
//NVSDK_NGX_VULKAN_Shutdown();
//// camera info
//
//params->Set(NVSDK_NGX_Parameter_Jitter_Offset_X, jitterX);
//
#endif //MYPROJECT_DLSS_HPP
