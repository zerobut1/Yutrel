set_project("Yutrel")

set_defaultarchs("x64")
set_warnings("all")

add_rules("mode.debug", "mode.release")

set_languages("c++17")
set_toolchains("clang")

-- 通用
add_requires("glfw","spdlog","stb")

-- Vulkan
add_requires("vulkansdk","vk-bootstrap","vulkan-memory-allocator")

-- imgui
-- add_requires("imgui v1.89.8-docking", {configs = {glfw_vulkan = true}})
add_requires("imgui", {configs = {glfw_vulkan = true}})

includes("Engine","Sandbox")