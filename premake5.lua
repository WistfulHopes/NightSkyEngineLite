
newoption
{
    trigger = "graphics",
    value = "OPENGL_VERSION",
    description = "version of OpenGL to build raylib against",
    allowed = {
        { "opengl11", "OpenGL 1.1"},
        { "opengl21", "OpenGL 2.1"},
        { "opengl33", "OpenGL 3.3"},
        { "opengl43", "OpenGL 4.3"}
    },
    default = "opengl33"
}

function define_C()
    language "C"
end

function define_Cpp()
    language "C++"
end

function string.starts(String,Start)
    return string.sub(String,1,string.len(Start))==Start
end

function link_to(lib)
    links (lib)
    includedirs ("../"..lib.."/include", "../"..lib.."/" )
end

function download_progress(total, current)
    local ratio = current / total;
    ratio = math.min(math.max(ratio, 0), 1);
    local percent = math.floor(ratio * 100);
    print("Download progress (" .. percent .. "%/100%)")
end

function check_raylib()
    if(os.isdir("raylib") == false and os.isdir("raylib") == false) then
        if(not os.isfile("raylib.zip")) then
            print("Raylib not found, downloading from github")
            local result_str, response_code = http.download("https://github.com/raysan5/raylib/archive/refs/heads/master.zip", "raylib.zip", {
                progress = download_progress,
                headers = { "From: Premake", "Referer: Premake" }
            })
        end
        print("Unzipping to " ..  os.getcwd())
        zip.extract("raylib.zip", os.getcwd())
        os.remove("raylib.zip")
    end
end

function check_rres()
    if(os.isdir("rres") == false and os.isdir("rres") == false) then
        if(not os.isfile("rres.zip")) then
            print("rres not found, downloading from github")
            local result_str, response_code = http.download("https://github.com/WistfulHopes/rres/archive/refs/heads/master.zip", "rres.zip", {
                progress = download_progress,
                headers = { "From: Premake", "Referer: Premake" }
            })
        end
        print("Unzipping to " ..  os.getcwd())
        zip.extract("rres.zip", os.getcwd())
        os.remove("rres.zip")
    end
end

function check_ggpo()
    if(os.isdir("ggpo") == false and os.isdir("ggpo") == false) then
        if(not os.isfile("ggpo.zip")) then
            print("ggpo not found, downloading from github")
            local result_str, response_code = http.download("https://github.com/WistfulHopes/ggpo/archive/refs/heads/master.zip", "ggpo.zip", {
                progress = download_progress,
                headers = { "From: Premake", "Referer: Premake" }
            })
        end
        print("Unzipping to " ..  os.getcwd())
        zip.extract("ggpo.zip", os.getcwd())
        os.remove("ggpo.zip")
    end
end

function check_NightSkyEngineCore()
    if(os.isdir("NightSkyEngineCore") == false and os.isdir("NightSkyEngineCore") == false) then
        if(not os.isfile("NightSkyEngineCore.zip")) then
            print("NightSkyEngineCore not found, downloading from github")
            local result_str, response_code = http.download("https://github.com/WistfulHopes/NightSkyEngineCore/archive/refs/heads/master.zip", "NightSkyEngineCore.zip", {
                progress = download_progress,
                headers = { "From: Premake", "Referer: Premake" }
            })
        end
        print("Unzipping to " ..  os.getcwd())
        zip.extract("NightSkyEngineCore.zip", os.getcwd())
        os.remove("NightSkyEngineCore.zip")
    end
end

workspaceName = path.getbasename(os.getcwd())

if (string.lower(workspaceName) == "raylib") then
    print("raylib is a reserved name. Name your project directory something else.")
    -- Project generation will succeed, but compilation will definitely fail, so just abort here.
    os.exit()
end

if (string.lower(workspaceName) == "rres") then
    print("rres is a reserved name. Name your project directory something else.")
    -- Project generation will succeed, but compilation will definitely fail, so just abort here.
    os.exit()
end

if (string.lower(workspaceName) == "ggpo") then
    print("ggpo is a reserved name. Name your project directory something else.")
    -- Project generation will succeed, but compilation will definitely fail, so just abort here.
    os.exit()
end

if (string.lower(workspaceName) == "NightSkyEngineCore") then
    print("NightSkyEngineCore is a reserved name. Name your project directory something else.")
    -- Project generation will succeed, but compilation will definitely fail, so just abort here.
    os.exit()
end

workspace (workspaceName)
    configurations { "Debug", "Release"}
    platforms { "x64", "x86"}

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"

    filter { "platforms:x64" }
        architecture "x86_64"

    filter {}

    targetdir "_bin/%{cfg.buildcfg}/"

    if(os.isdir("source")) then
        startproject(workspaceName)
    end

    cdialect "C99"
    cppdialect "C++11"
    
check_raylib();
check_rres();
check_ggpo();
check_NightSkyEngineCore();

include ("raylib_premake5.lua")
include ("rres_premake5.lua")
include ("ggpo_premake5.lua")
include ("NightSkyEngineCore_premake5.lua")

if(os.isdir("source")) then
    include ("source")
end

folders = os.matchdirs("*")
for _, folderName in ipairs(folders) do
    if (string.starts(folderName, "raylib") == false and string.starts(folderName, "_") == false and string.starts(folderName, ".") == false) then
        if (os.isfile(folderName .. "/premake5.lua")) then
            print(folderName)
            include (folderName)
        end
    end
    if (string.starts(folderName, "rres") == false and string.starts(folderName, "_") == false and string.starts(folderName, ".") == false) then
        if (os.isfile(folderName .. "/premake5.lua")) then
            print(folderName)
            include (folderName)
        end
    end
    if (string.starts(folderName, "ggpo") == false and string.starts(folderName, "_") == false and string.starts(folderName, ".") == false) then
        if (os.isfile(folderName .. "/premake5.lua")) then
            print(folderName)
            include (folderName)
        end
    end
    if (string.starts(folderName, "NightSkyEngineCore") == false and string.starts(folderName, "_") == false and string.starts(folderName, ".") == false) then
        if (os.isfile(folderName .. "/premake5.lua")) then
            print(folderName)
            include (folderName)
        end
    end
end
