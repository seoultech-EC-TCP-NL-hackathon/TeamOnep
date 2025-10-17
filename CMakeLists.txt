cmake_minimum_required(VERSION 3.10)
project(MyProject LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(SPDLOG_COMPILED_LIB ON)
add_definitions(-DSPDLOG_COMPILED_LIB)
if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Od")
    add_compile_options(/utf-8)
    add_compile_options(
            /wd4018  # signed/unsigned 비교
            /wd4267  # size_t -> int 변환
            /wd2666  # 오버로드 충돌
    )
    execute_process(COMMAND chcp 65001)
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
endif ()
set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/source)
set(EXTERN_DIR ${CMAKE_SOURCE_DIR}/extern)
set(GLM_DIR ${CMAKE_SOURCE_DIR}/extern/glm)
set(EXTERN_DIR ${CMAKE_SOURCE_DIR}/extern)
set(MODEL_DIR ${EXTERN_DIR}/externModel/assets/models/)
set(TEXTURE_DIR ${EXTERN_DIR}/externModel/assets/textures)
add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
add_compile_options(/wd4005 /wd4996 /wd4834)
include_directories(
        ${SOURCE_DIR}/resource
        ${SOURCE_DIR}/renderer
        ${SOURCE_DIR}/model
        ${SOURCE_DIR}/core
        ${SOURCE_DIR}/core/GPU
        ${SOURCE_DIR}/core/GPU/vk_backend
        ${SOURCE_DIR}/core/util
        ${SOURCE_DIR}/core/io
        ${SOURCE_DIR}/scene_graph

        ${SOURCE_DIR}/feature/sculptor
        ${SOURCE_DIR}/feature/UI

        ${EXTERN_DIR}
        ${EXTERN_DIR}/imgui
        ${EXTERN_DIR}/imgui/back

        ${EXTERN_DIR}/ktx/lib
        ${EXTERN_DIR}/ktx/include
        ${EXTERN_DIR}/ktx/other_include
)
find_package(assimp REQUIRED)
find_package(spdlog REQUIRED)
find_package(Vulkan REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(unofficial-shaderc CONFIG REQUIRED PATHS "C:/Users/dlwog/vcpkg/installed/x64-windows/share/shaderc")
option(SUB_DIR "SUB_DIR" ON)

set(VK_BACKEND ${CMAKE_SOURCE_DIR}/source/core/GPU/vk_backend)
set(KTX_DIR ${CMAKE_SOURCE_DIR}/source/core/GPU/ktx)
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/source/core/GPU/imgui)
set(IMNODE_DIR ${CMAKE_SOURCE_DIR}/source/core/GPU/imnodes)
add_definitions(-DSPDLOG_HEADER_ONLY)


include_directories(
        ${VK_BACKEND}/
        ${VK_BACKEND}/../../util
        ${IMGUI_DIR}/
        ${IMGUI_DIR}/back
        ${IMGUI_DIR}/back/vulkan
        ${IMNODE_DIR}/
        ${KTX_DIR}/include
        ${KTX_DIR}/lib
)
# 공통 소스 파일
set(COMMON_SRC
        main.cpp
        source/core/engine.cpp
        source/core/resource_pool_init.cpp
        source/core/resource_pool.cpp

        source/core/render_graph/renderer.cpp

        source/core/importer.cpp

        source/core/io/io.cpp
        source/core/io/log_sink.cpp
        source/core/io/event_manager.cpp
        source/resource/descriptor_manager.cpp
        source/resource/descriptor_pool.cpp
        source/resource/shader_pool.cpp
        source/resource/static_buffer.cpp
        source/resource/descriptor_uploader.cpp
        source/resource/buffer_builder.cpp
        source/resource/texture.cpp
        source/resource/swapchain_view.cpp

        source/core/scene_graph/camera.cpp
        source/core/scene_graph/mesh.cpp

        source/resource/semaphore_pool.cpp
        source/resource/pipeline_pool.cpp

        source/feature/sculptor/sculptor.cpp
        source/feature/sculptor/dynamic_mesh.cpp
        source/feature/sculptor/sculptor_act.cpp

        source/feature/UI/ui.cpp

        source/resource/command_pool.cpp
        source/resource/semaphore_pool.cpp


        ${VK_BACKEND}/vk_command_buffer.cpp
        ${VK_BACKEND}/vk_sync_object.cpp
        ${VK_BACKEND}/vk_context.cpp
        ${VK_BACKEND}/vk_scheduler.cpp
        ${VK_BACKEND}/vk_graph_builder.cpp
        ${VK_BACKEND}/vk_graph_compiler.cpp
        ${VK_BACKEND}/vk_resource_allocator.cpp
        ${VK_BACKEND}/vk_memory_allocator.cpp
        ${VK_BACKEND}/vk_memory_pool.cpp
        ${VK_BACKEND}/vk_graph_executer.cpp
        ${VK_BACKEND}/vk_swapchain.cpp
        ${VK_BACKEND}/vk_descriptor_allocator.cpp
        ${VK_BACKEND}/vk_sampler_builder.cpp

        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_impl_glfw.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/back/imgui_impl_vulkan.cpp

        source/resource/sampler_builder.cpp

        source/core/GPU/context.cpp
        source/core/GPU/vk_backend/vk_discard_pool.cpp
)
message(STATUS "Vulkan include: ${Vulkan_INCLUDE_DIRS}")
add_executable(exe ${COMMON_SRC})

# OS별 링크 및 정의
if (WIN32)
    target_link_libraries(exe
            glfw
            Vulkan::Vulkan
            spdlog::spdlog
            spdlog::spdlog_header_only
            fmt::fmt
            assimp::assimp
            unofficial::shaderc::shaderc
    )
else ()
    target_link_libraries(exe
            glfw
            vulkan
            vk_imple
            dl
            pthread
            X11
            Xxf86vm
            Xrandr
            Xi
            shaderc
            spdlog::spdlog
            assimp::assimp
            imnode
            ktx
    )
endif ()


target_include_directories(exe PRIVATE ${Vulkan_INCLUDE_DIRS})
target_compile_definitions(exe PRIVATE
        ASSET_MODELS_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/assets/models/\"
        ASSET_TEXTURES_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/assets/textures/\"
        PIPELINE_CACHE_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/extern/pipelineCache/\"
        SPDLOG_HEADER_ONLY
        BUILD_SHARED_LIBS=OFF/SPDLOG_BUILD_SHARED=OFF
)

target_compile_options(exe PRIVATE /W0)
target_compile_definitions(exe PRIVATE FORCE_UTF8_CONSOLE)
