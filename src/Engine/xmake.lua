target("YutrelEngine")
    set_kind("static")
    set_toolchains("clang")

    add_files("$(projectdir)/src/Engine/**.cpp")
    add_headerfiles("$(projectdir)/src/Engine/**.h")

    add_includedirs("$(projectdir)/src/Engine/", {public = true})

    add_packages("spdlog","glfw")

    add_packages("vulkansdk","vk-bootstrap","vulkan-memory-allocator-hpp")

    -- add_defines("GLM_FORCE_DEPTH_ZERO_TO_ONE")

    -- 着色器
    -- add_rules("utils.glsl2spv",{bin2c = true})
    -- add_files("asset/shader/**.vert","asset/shader/**.frag","asset/shader/**.comp")
    -- add_packages("glslang")