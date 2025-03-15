set_project("Yutrel")

set_defaultarchs("x64")
set_warnings("all")

add_rules("mode.debug", "mode.release")

set_languages("c++17")

-- 通用
add_requires("glfw","spdlog","stb")

-- Vulkan
add_requires("vulkansdk","vk-bootstrap","vulkan-memory-allocator-hpp")
add_requires("glslang", {configs = {binaryonly = true}})

-- imgui
add_requires("imgui v1.89.8-docking", {configs = {glfw_vulkan = true}})

includes("Engine","Sandbox")