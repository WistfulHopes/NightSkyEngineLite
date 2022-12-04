function platform_defines()
    defines{"PLATFORM_DESKTOP"}

    filter {"system:linux"}
        defines {"_GNU_SOURCE"}
-- This is necessary, otherwise compilation will fail since
-- there is no CLOCK_MONOTOMIC. ggpo claims to have a workaround
-- to compile under c99 without -D_GNU_SOURCE, but it didn't seem
-- to work. ggpo's Makefile also adds this flag, probably why it went
-- unnoticed for so long.
-- It compiles under c11 without -D_GNU_SOURCE, because c11 requires
-- to have CLOCK_MONOTOMIC
-- See: https://github.com/raysan5/ggpo/issues/2729

    filter{}
end

function get_ggpo_dir()
    if (os.isdir("ggpo")) then
        return "ggpo"
    end
    if (os.isdir("../ggpo")) then
        return "ggpo"
    end
    return "ggpo"
end

function link_ggpo()
    links {"ggpo"}

    ggpo_dir = get_ggpo_dir();
    includedirs {"../" .. ggpo_dir .. "/src" }
    platform_defines()

    filter "action:vs*"
        defines{"_WINSOCK_DEPRECATED_NO_WARNINGS", "_CRT_SECURE_NO_WARNINGS"}
        dependson {"ggpo"}
        links {"ggpo.lib"}
        characterset ("MBCS")

    filter "system:windows"
        defines{"_WIN32"}
        links {"winmm", "kernel32"}
        libdirs {"../_bin/%{cfg.buildcfg}"}

    filter "system:linux"
        links {"pthread", "m", "dl", "rt", "X11"}
        
    filter{}
end

function include_ggpo()
    ggpo_dir = get_ggpo_dir();
    includedirs {"../" .. ggpo_dir .."/src/include" }
    includedirs {"../" .. ggpo_dir .."/src/lib/ggpo" }
    includedirs {"../" .. ggpo_dir .."/src/lib/ggpo/network" }
    includedirs {"../" .. ggpo_dir .."/src/lib/ggpo/backends" }
    platform_defines()

    filter "action:vs*"
        defines{"_WINSOCK_DEPRECATED_NO_WARNINGS", "_CRT_SECURE_NO_WARNINGS"}

    filter{}
end

project "ggpo"
    kind "StaticLib"

    platform_defines()

    location "_build"
    language "C++"
    targetdir "_bin/%{cfg.buildcfg}"

    filter "action:vs*"
        defines{"_WINSOCK_DEPRECATED_NO_WARNINGS", "_CRT_SECURE_NO_WARNINGS"}
        characterset ("MBCS")

    filter{}

    ggpo_dir = get_ggpo_dir();
    print ("Using ggpo dir " .. ggpo_dir);
    includedirs {ggpo_dir .. "/src/include" , ggpo_dir .. "/src/lib/ggpo/", ggpo_dir .. "/src/lib/ggpo/network" , ggpo_dir .. "/src/lib/ggpo/backends"}
    vpaths
    {
        ["Header Files"] = { ggpo_dir .. "/src/include/**.h", ggpo_dir .. "/src/lib/ggpo/include/**.h", ggpo_dir .. "/src/lib/ggpo/network/**.h", ggpo_dir .. "/src/lib/ggpo/backends/**.h",},
        ["Source Files/*"] = { ggpo_dir .. "/src/lib/ggpo/**.cpp", ggpo_dir .. "/src/lib/ggpo/network/**.cpp", ggpo_dir .. "/src/lib/ggpo/backends/**.cpp"}
    }
    files {ggpo_dir .. "/src/include/**.h", ggpo_dir .. "/src/lib/ggpo/include/**.h", ggpo_dir .. "/src/lib/ggpo/network/**.h", ggpo_dir .. "/src/lib/ggpo/backends/**.h", ggpo_dir .. "/src/lib/ggpo/**.cpp", ggpo_dir .. "/src/lib/ggpo/network/**.cpp", ggpo_dir .. "/src/lib/ggpo/backends/**.cpp"}

    filter{}
