//
// Created by ljh on 25. 9. 27..
//

#ifndef MYPROJECT_BOOT_RESOURCE_HPP
#define MYPROJECT_BOOT_RESOURCE_HPP

#include <string>
#include <cstdint>

namespace gpu
{
  using RenderNode = gpu::VkResourceId;

  std::string fragPath     = "/home/ljh/CLionProjects/VkMain/source/shader/fragment.frag";
  std::string vertPath     = "/home/ljh/CLionProjects/VkMain/source/shader/vertex.vert";
  std::string fragBackPath = "/home/ljh/CLionProjects/VkMain/source/shader/dftBackground.frag";
  std::string VertBackPath = "/home/ljh/CLionProjects/VkMain/source/shader/quad.vert";

  enum class DescriptorFlag : uint32_t{
    UBO                    = 0x00000001,
    DYNAMIC_UBO            = 0x00000002,
    SSBO                   = 0x00000004,
    SAMPLER                = 0x00000008,
    COMBINED_IMAGE_SAMPLER = 0x00000010,
    TEXTURE                = 0x00000040,
    TEXTURE_BINDLESS       = 0x00000080,
    NOT_DEFINED            = 0x00000100,
    SHADER_VERTEX          = 0x00000200,
    SHADER_GEOMETRY        = 0x00000400,
    SHADER_FRAGMENT        = 0x00000800,
    SHADER_COMPUTE         = 0x00001000,
    BASE                   = 0x00002000,
  }; //can append raytracing shader structure

  constexpr uint32_t VERTEX_BINDING   = 0;
  constexpr uint32_t INDEX_BINDING    = 0;
  constexpr uint32_t INSTANCE_BINDING = 1;

  constexpr uint32_t GLOBAL_LAYOUT    = 0;
  constexpr uint32_t BINDLESS_TEXTURE = 0;
  constexpr uint32_t CAMERA_BINDING   = 1;
  constexpr uint32_t GLOBAL_LIGHT     = 2;
  constexpr uint32_t LOCAL_LIGHT      = 3;
  constexpr uint32_t SHADOW_LAYOUT    = 4;


  constexpr uint32_t TEXTURE_LAYOUT    = 1;
  constexpr uint32_t ALBEDO_BINDING    = 0;
  constexpr uint32_t NORMAL_BINDING    = 1;
  constexpr uint32_t ROUGHNESS_BINDING = 2;

  constexpr uint32_t BINDELSS_TEXTURE_ARRAY_COUNT = 1024;
  constexpr uint32_t DYNAMIC_UBO_ARRAY_COUNT      = 8;

  inline uint32_t operator&(uint32_t a, DescriptorFlag b)
  {
    return a & static_cast<unsigned int>(b);
  }

  inline constexpr uint32_t operator|(DescriptorFlag a, DescriptorFlag b)
  {
    return static_cast<unsigned int>(a) | static_cast<unsigned int>(b);
  }

  inline constexpr uint32_t operator|(uint32_t a, DescriptorFlag b)
  {
    return a | static_cast<unsigned int>(b);
  }
}
#endif //MYPROJECT_BOOT_RESOURCE_HPP
