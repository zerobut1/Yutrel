target("Yutrel")
    add_deps("runtime")
    set_kind("binary")

    set_pcxxheader("$(projectdir)/Engine/src/yutrel_pch.h")

    add_packages("spdlog","imgui","glm")

    add_headerfiles("**.h")
    add_files("**.cpp")

    add_includedirs("../")