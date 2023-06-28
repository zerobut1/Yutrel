target("Yutrel")
    set_kind("static")

    set_pcxxheader("$(projectdir)/Engine/src/Yutrel/core/pch/yutrel_pch.hpp")

    add_packages("glfw","glad","glm","assimp","spdlog","imgui")

    add_headerfiles("**.h")
    add_headerfiles("**.hpp")
    add_headerfiles("$(projectdir)/Engine/vendor/stb_image/*.h")
    add_files("**.cpp")
    add_files("$(projectdir)/Engine/vendor/stb_image/*.cpp")

    add_includedirs("../")
    add_includedirs("../Yutrel/core/pch")
    add_includedirs("$(projectdir)/Engine/vendor/stb_image")