--- GENERAL
add_rules("mode.debug", "mode.release"); set_defaultmode("debug")
add_rules("plugin.compile_commands.autoupdate")


if is_mode("debug") then
    set_optimize("none")
    set_symbols("debug")
elseif is_mode("release") then
    add_defines("DEBUG_ON=0")
    set_optimize("fastest")
    set_symbols("none")
    set_strip("all")
end


--- GLOBAL
add_includedirs(".", "include/")
set_languages("c++23")
add_cxxflags("-Wall")

--- TESTS
target("main") add_files("tests/main.cpp")

--- TARGETS
target("dotline")
    set_kind("headeronly")
    set_languages("c++17")
    add_headerfiles("include/dotline/*.hpp")
