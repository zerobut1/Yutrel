set_project("Yutrel")

set_defaultarchs("x64")
set_warnings("all")

add_rules("mode.debug", "mode.release")

set_languages("c++17")

-- 使用msvc时加上
-- add_cxxflags("/utf-8")

-- 通用
add_requires("glfw","glm","spdlog","stb","assimp","tinyobjloader")

-- OpenGL
add_requires("glad")

-- Vulkan
add_requires("vulkansdk","vk-bootstrap","vulkan-memory-allocator")
add_requires("glslang", {configs = {binaryonly = true}})

-- imgui
add_requires("imgui v1.90-docking")

includes("engine","sandbox")