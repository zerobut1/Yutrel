add_requires("glfw","glad","glm","assimp")

target("runtime")
    set_kind("static")
    add_packages("glfw","glad","glm","assimp")

    add_includedirs("./")
    add_files("runtime/**.cpp")

target("editor")
    add_deps("runtime")
    set_kind("binary")

    add_includedirs("./")
    add_files("editor/**.cpp")