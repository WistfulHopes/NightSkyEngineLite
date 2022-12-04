
function platform_defines()
    defines{"PLATFORM_DESKTOP"}

    filter {"system:linux"}
        defines {"_GNU_SOURCE"}
-- This is necessary, otherwise compilation will fail since
-- there is no CLOCK_MONOTOMIC. rres claims to have a workaround
-- to compile under c99 without -D_GNU_SOURCE, but it didn't seem
-- to work. rres's Makefile also adds this flag, probably why it went
-- unnoticed for so long.
-- It compiles under c11 without -D_GNU_SOURCE, because c11 requires
-- to have CLOCK_MONOTOMIC
-- See: https://github.com/raysan5/rres/issues/2729

    filter{}
end

function get_rres_dir()
    if (os.isdir("rres")) then
        return "rres"
    end
    if (os.isdir("../rres")) then
        return "rres"
    end
    return "rres"
end

function include_rres()
    rres_dir = get_rres_dir();
    includedirs {"../" .. rres_dir .."/src" }
    includedirs {"../" .. rres_dir .."/src/external" }
    platform_defines()

    filter "action:vs*"
        defines{"_WINSOCK_DEPRECATED_NO_WARNINGS", "_CRT_SECURE_NO_WARNINGS"}

    filter{}
end

project "rres"
    kind "StaticLib"

    platform_defines()

    location "_build"
    language "C"
    targetdir "_bin/%{cfg.buildcfg}"

    filter "action:vs*"
        defines{"_WINSOCK_DEPRECATED_NO_WARNINGS", "_CRT_SECURE_NO_WARNINGS"}
        characterset ("MBCS")

    filter{}

    rres_dir = get_rres_dir();
    print ("Using rres dir " .. rres_dir);
    includedirs {rres_dir .. "/src" }
    vpaths
    {
        ["Header Files"] = { rres_dir .. "/src/**.h"},
        ["Source Files/*"] = { rres_dir .. "/src/**.c"},
    }
    files {rres_dir .. "/src/*.h", rres_dir .. "/src/*.c"}

    filter{}
