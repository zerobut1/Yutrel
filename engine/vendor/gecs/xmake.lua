target("gecs")
    set_kind("static")
    set_toolchains("clang")

    add_headerfiles("src/**.hpp")

    add_includedirs(path.join(os.scriptdir(), "src"), {public = true})