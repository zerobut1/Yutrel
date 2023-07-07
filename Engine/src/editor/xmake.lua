target("Yutrel_Editor")
    add_deps("Yutrel")
    set_kind("binary")

    add_packages("spdlog","imgui","glm")
    
    -- tmp
    add_packages("glfw","glad","assimp","entt")

    add_headerfiles("**.hpp")
    add_files("**.cpp")

    add_includedirs("../")