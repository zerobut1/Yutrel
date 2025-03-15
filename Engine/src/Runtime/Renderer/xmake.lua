target("Renderer")
    set_kind("static")

    add_headerfiles("**.h")
    add_files("**.cpp")

    add_includedirs("./Public", { public = true })

    add_deps("Core","Window")

    add_packages(
        "vulkansdk",
        "vulkan-memory-allocator-hpp",
        "spdlog",
        "glfw",
        "vk-bootstrap"
    )

    -- add_defines("GLM_FORCE_DEPTH_ZERO_TO_ONE")

    -- 着色器
    -- add_rules("utils.glsl2spv",{bin2c = true})
    -- add_files("asset/shader/**.vert","asset/shader/**.frag","asset/shader/**.comp")
    -- add_packages("glslang")