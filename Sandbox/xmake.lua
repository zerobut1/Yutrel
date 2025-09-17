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

    add_packages("glm", "imgui")

    -- shader
    add_rules("utils.hlsl2spv", {bin2c = true})
    add_files("shader/*.cs.hlsl")
    add_headerfiles("shader/**.hlsli")
    add_packages("directxshadercompiler")