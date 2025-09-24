target("Renderer")
    set_kind("static")

    add_headerfiles("**.h")
    add_files("**.cpp")

    add_includedirs("./Public", { public = true })

    add_deps("Core","Window")

    add_packages(
        "vulkansdk",
        "vulkan-memory-allocator",
        "vk-bootstrap",
        { public = true }
    )