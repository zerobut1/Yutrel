target("runtime")
    set_kind("static")
    add_packages("glfw","glad","glm","assimp","spdlog")

    add_headerfiles("**.h")
    add_files("**.cpp")

    add_includedirs("../")