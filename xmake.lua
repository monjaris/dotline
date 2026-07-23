--- UNIVERSAL
set_arch("x86_64")
set_plat("linux")

--- RULES/POLICIES
add_rules("mode.debug", "mode.release"); set_defaultmode("debug")
add_rules("plugin.compile_commands.autoupdate")

--- TOOLCHAIN
toolchain("dotline.gnu")
    set_kind("standalone"); set_toolset("cxx", "g++")
    set_toolset("as",    "as"); set_toolset("ar",    "ar")
    set_toolset("ld",    "g++"); set_toolset("sh",    "g++")
    set_toolset("ex",    "g++"); set_toolset("strip", "strip")
toolchain_end()

set_toolchains("dotline.gnu")

--- SCRIPT-BEGIN
    if is_mode("debug") then
        set_optimize("none")
        set_symbols("debug")
    elseif is_mode("release") then
        add_defines("DEBUG_ON=0")
        set_optimize("fastest")
        set_symbols("none")
        set_strip("all")
    end
--- SCRIPT-END

--- GLOBAL
add_includedirs(".", "include/")
set_languages("c++23")

--- TESTS
target("main") add_files("tests/main.cpp")

--- TARGETS
target("dotline")
    set_kind("headeronly")
    set_languages("c++17")
    add_headerfiles("include/**.hpp")
