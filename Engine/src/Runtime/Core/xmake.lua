target("Core")
    set_kind("static")

    add_headerfiles("**.h")
    add_files("**.cpp")

    add_includedirs("./Public", { public = true })
    
    add_packages("spdlog","vulkansdk")