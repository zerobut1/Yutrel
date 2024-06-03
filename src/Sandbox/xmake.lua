target("Sandbox")
    set_kind("binary")
    set_toolchains("clang")

    set_rundir("$(projectdir)")

    add_files("$(projectdir)/src/Sandbox/**.cpp")
    add_headerfiles("$(projectdir)/src/Sandbox/src/**.hpp")

    add_deps("YutrelEngine")

    add_includedirs("$(projectdir)/src/Engine")