target("Gui")
    set_kind("static")

    add_headerfiles("**.h")
    add_files("**.cpp")

    add_includedirs("./Public", { public = true })

    add_deps("Core","Renderer","Window")

    add_packages("glfw","vulkansdk","imgui", {public = true})