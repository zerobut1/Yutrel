set_project("Yutrel")

set_defaultarchs("x64")
set_warnings("all")

add_rules("mode.debug", "mode.release")

set_languages("c++20")
set_encodings("utf-8")

-- 通用
add_requires("spdlog", "stb", "glm")

-- glfw
add_requires("glfw")

-- Vulkan
add_requires("vulkansdk", "vk-bootstrap v1.4.320", "vulkan-memory-allocator")

-- imgui
add_requires("imgui v1.92.1-docking", {configs = {glfw = true, vulkan = true}})

-- shader
add_requires("glslang", {configs = {binaryonly = true}})

includes("Engine", "Sandbox")