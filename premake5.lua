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

    targetdir ("bin/" .. outputdir .. "/%{cfg.architecture}")
    objdir ("bin-int/" .. outputdir .. "/%{cfg.architecture}")

    files
    {
        "source/**.h",
        "source/**.cpp"
    }

    includedirs
    {
        "vendor/glew/include/",
        "vendor/openal-soft/include/",
        "vendor/assimp/include/",
        "vendor/freealut/include/",
        "vendor/openal-soft/include/",
        "vendor/gli/",
        "vendor/glm/",
        "vendor/libdevil/include/",
		"vendor/SDL2/include/"
    }

	libdirs
	{
		"vendor/glew/lib/Release/x64/",
		"vendor/assimp/lib/Debug/",
		"vendor/SDL2/lib/x64/",
		"vendor/openal-soft/Debug/"
	}
    links
    {
		"glew32.lib",
		"assimp-vc140-mt.lib",
		"SDL2.lib",
		"SDL2main.lib",
		"OpenAL32.lib",
		"OpenGL32.lib"		
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
    

--project "OpenAL"
--    location "vendor/openal-soft"
