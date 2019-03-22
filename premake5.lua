workspace "OpenGLProject"
    architecture    "x64"
    
    configurations
    {
        "Debug",
        "Release",
        "ReleaseDist"
    }

outputdir = "%{cfg.buildcfg}"

project "OpenGLProject"
    kind "ConsoleApp"
	toolset ("v140")
    language "C++"
	pchheader "stdafx.h"
	pchsource "source/stdafx.cpp"

    targetdir ("bin/" .. outputdir .. "/%{cfg.architecture}")
    objdir ("bin-int/" .. outputdir .. "/%{cfg.architecture}")

    files
    {
        "source/**.h",
        "source/**.cpp",
		"shaders/**.glsl"
    }

	vpaths
	{
	 	["Headers"] = "**.h",
		["Sources"] = "**.cpp",
		["Shaders"] = "**.glsl"
	}

    includedirs
    {
        "vendor/glew/include/",
        "vendor/AL/include/",
        "vendor/assimp/include/",
        "vendor/gli/",
        "vendor/glm/",
        "vendor/IL/include/",
		"vendor/SDL2/include/",
		"vendor/SDL2_image/include/",
		"vendor/spdlog/include/"
    }

	libdirs
	{
		"vendor/glew/lib/Release/x64/",
		"vendor/assimp/lib/",
		"vendor/SDL2/lib/x64/",
		"vendor/SDL2_image/lib/x64/",
		"vendor/AL/lib/Win64/",
		"vendor/IL/"
	}

    links
    {
		"opengl32.lib",
		"glew32.lib",
		"assimp-vc140-mt.lib",
		"SDL2.lib",
		"SDL2main.lib",
		"SDL2_image.lib",
		"DevIL.lib",
		"ILU.lib",
		"ILUT.lib",
		"OpenAL32.lib"
    }

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


