includes("vendor")

target("engine")
    set_kind("static")
    set_toolchains("clang")

    add_files("src/**.cpp")
    add_headerfiles("src/**.hpp")

    -- 通用
    add_packages("glfw","glm","spdlog","stb","assimp")
    -- OpenGL
    add_packages("glad")
    -- Vulkan
    add_packages("vulkansdk","vk-bootstrap","vulkan-memory-allocator")
    -- imgui
    add_packages("imgui")

    add_deps("gecs")

    add_includedirs(path.join(os.scriptdir(), "src"), {public = true})

    set_pcxxheader(path.join(os.scriptdir(), "src", "core", "pch", "yutrel_pch.hpp"))
    add_includedirs(path.join(os.scriptdir(), "src", "core", "pch"))