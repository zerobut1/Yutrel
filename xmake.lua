-- define project
set_project("Yutrel")

-- add build modes
set_defaultarchs("x64")
add_rules("mode.debug")
-- add_rules("plugin.vsxmake.autoupdate")

-- includes sub-projects
includes("Engine/src")

set_targetdir("build")