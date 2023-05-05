target("editor")
    add_deps("runtime")
    set_kind("binary")
    add_packages("spdlog","imgui")

    add_headerfiles("**.h")
    add_files("**.cpp")

    add_includedirs("../")