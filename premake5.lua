workspace "OpenGLProject"
    architecture    "x64"
    
    configurations
    {
        "Debug",
        "Release",
        "ReleaseDist"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "OpenGLProject"
    kind "ConsoleApp"
    language "C++"

    targetdir ("bin/" .. outputdir .. "%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "%{prj.name}")

    files
    {
        "source/**.h",
        "source/**.cpp"
    }

    includedirs
    {
        "bin/vendor/glew/include",
        "vendor/openal-soft/include/",
        "vendor/assimp/include/",
        "vendor/freealut/include/",
        "vendor/openal-soft/include/",
        "vendor/gli/",
        "vendor/glm/",
        "vendor/libdevil/include"
    }

--    links
--    {
--
--    }

    filter "system:windows"
        cppdialect "C++17"
        systemversion "latest"

        defines
        {
            "",
            ""
        } 
        
--        postbuildcommands
--        {
--            ("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "")
--        }
    filter "configurations:Debug"
        defines ""
        symbols "On"
    
    filter "configurations:Release"
        defines ""
        symbols "On"
    
    filter "configurations:Dist"
        defines ""
        optimize "On"
    

--project "OpenAL"
--    location "vendor/openal-soft"
