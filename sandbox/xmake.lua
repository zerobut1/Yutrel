target("sandbox")
    set_kind("binary")
    set_toolchains("clang")

    set_rundir("$(projectdir)")

    add_files("src/**.cpp")
    add_headerfiles("src/**.hpp")

    add_deps("engine")

    add_packages("spdlog","glm","assimp","imgui","tinygltf")

    add_includedirs(path.join(os.scriptdir(), "src"))