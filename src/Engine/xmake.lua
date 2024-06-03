target("YutrelEngine")
    set_kind("static")
    set_toolchains("clang")

    add_files("$(projectdir)/src/Engine/**.cpp")
    add_headerfiles("$(projectdir)/src/Engine/**.hpp")

    -- add_includedirs(path.join(os.scriptdir(), "src"), {public = true})

    -- set_pcxxheader(path.join(os.scriptdir(), "src", "core", "pch", "yutrel_pch.hpp"))
    -- add_includedirs(path.join(os.scriptdir(), "src", "core", "pch"))

    -- add_defines("GLM_FORCE_DEPTH_ZERO_TO_ONE")

    -- 着色器
    -- add_rules("utils.glsl2spv",{bin2c = true})
    -- add_files("asset/shader/**.vert","asset/shader/**.frag","asset/shader/**.comp")
    -- add_packages("glslang")