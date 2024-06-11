target("Sandbox")
    set_kind("binary")
    set_toolchains("clang")

    set_rundir("$(projectdir)")

    add_files("$(projectdir)/src/Sandbox/**.cpp")
    add_headerfiles("$(projectdir)/src/Sandbox/src/**.h")

    add_deps("YutrelEngine")

    -- add_includedirs("$(projectdir)/src/Engine")

    add_packages("spdlog")

    add_packages("vulkansdk", "vulkan-memory-allocator-hpp")