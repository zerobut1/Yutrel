target("Yutrel_Editor")
    add_deps("Yutrel")
    set_kind("binary")

    -- set_pcxxheader("$(projectdir)/Engine/src/yutrel_pch.h")

    add_packages("spdlog","imgui","glm")
    
    -- tmp
    add_packages("glfw","glad","assimp")

    add_headerfiles("**.h")
    add_files("**.cpp")

    add_includedirs("../")