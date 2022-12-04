function platform_defines()
    defines{"PLATFORM_DESKTOP"}

    filter {"system:linux"}
        defines {"__GNUC__"}
    
    filter{}
        
    filter {"system:windows"}
        defines {"_WINDOWS"}

    filter{}
end

function get_NightSkyEngineCore_dir()
    if (os.isdir("NightSkyEngineCore")) then
        return "NightSkyEngineCore"
    end
    if (os.isdir("../NightSkyEngineCore")) then
        return "NightSkyEngineCore"
    end
    return "NightSkyEngineCore"
end

function link_NightSkyEngineCore()
    links {"NightSkyEngineCore"}

    NightSkyEngineCore_dir = get_NightSkyEngineCore_dir();
    includedirs {"../" .. NightSkyEngineCore_dir .. "/src" }
    platform_defines()

    filter "action:vs*"
        defines{"_WINSOCK_DEPRECATED_NO_WARNINGS", "_CRT_SECURE_NO_WARNINGS"}
        dependson {"NightSkyEngineCore"}
        links {"NightSkyEngineCore.lib"}
        characterset ("MBCS")

    filter "system:windows"
        defines{"_WIN32"}
        links {"winmm", "kernel32"}
        libdirs {"../_bin/%{cfg.buildcfg}"}

    filter "system:linux"
        links {"pthread", "m", "dl", "rt", "X11"}
        
    filter{}
end

function include_NightSkyEngineCore()
    NightSkyEngineCore_dir = get_NightSkyEngineCore_dir();
    includedirs {"../" .. NightSkyEngineCore_dir .."/src/" }
    includedirs {"../" .. NightSkyEngineCore_dir .."/src/Battle" }
    includedirs {"../" .. NightSkyEngineCore_dir .."/src/Battle/Actors" }
    platform_defines()

    filter "action:vs*"
        defines{"_WINSOCK_DEPRECATED_NO_WARNINGS", "_CRT_SECURE_NO_WARNINGS"}

    filter{}
end

project "NightSkyEngineCore"
    kind "StaticLib"

    platform_defines()

    location "_build"
    language "C++"
    targetdir "_bin/%{cfg.buildcfg}"

    filter "action:vs*"
        defines{"_CRT_SECURE_NO_WARNINGS"}
        characterset ("MBCS")

    filter{}

    NightSkyEngineCore_dir = get_NightSkyEngineCore_dir();
    print ("Using NightSkyEngineCore dir " .. NightSkyEngineCore_dir);
    includedirs {NightSkyEngineCore_dir .. "/src/" , NightSkyEngineCore_dir .. "/src/Battle/", NightSkyEngineCore_dir .. "/src/Battle/Actors", NightSkyEngineCore_dir .. "/src/Battle/NightSkyScript"}
    files {NightSkyEngineCore_dir .. "/src/**.h", NightSkyEngineCore_dir .. "/src/**.cpp", NightSkyEngineCore_dir .. "/src/Battle/**.h", NightSkyEngineCore_dir .. "/src/Battle/**.cpp", NightSkyEngineCore_dir .. "/src/Battle/Actors/**.h", NightSkyEngineCore_dir .. "/src/Battle/Actors/**.cpp", NightSkyEngineCore_dir .. "/src/Battle/NightSkyScript/**.h", NightSkyEngineCore_dir .. "/src/Battle/NightSkyScript/**.cpp"}
       
    filter{}