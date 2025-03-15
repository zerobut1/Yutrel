target("Sandbox")
    set_kind("binary")

    set_rundir("$(projectdir)")

    add_files("**.cpp")
    add_headerfiles("**.h")

    add_deps(
        "Application",
        "Core",
        "Renderer",
        "Window"
    )

    add_packages("vulkansdk")