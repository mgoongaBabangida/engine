-- premake5.lua

workspace "Mgoonga"
	architecture "x64"
	configurations { "Debug", "Release" }
    startproject "Mgoonga"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

------------------------------------------------------------------------------------------------
project "base"
	location "base"
	kind "SharedLib"
	language "C++"
	cppdialect "C++17"
	targetdir ("bin/" .. outputdir .. "/")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	
   files { "%{prj.name}/**.h", "%{prj.name}/**.cpp"}
   
   includedirs { "../../third_party" }
	
   filter "configurations:Debug"
      defines { "DEBUG;_DEBUG;_WINDOWS;_USRDLL;BASE_EXPORTS" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG;_WINDOWS;_USRDLL;BASE_EXPORTS" }
      optimize "On"
	  
	  --postbuildcommands
	  --{
	  --("{COPY} %{cfg.buidtarget.relpath} ../bin/" .. outputdir .. "/Mgoonga")
	  --}
	  
------------------------------------------------------------------------------------------------
project "yaml-cpp"
	location "yaml-cpp"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	targetdir ("bin/" .. outputdir .. "/")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	
   files 
   {
   "%{prj.name}/src/**.h",
   "%{prj.name}/src/**.cpp",
   
   "%{prj.name}/include/**.h"
   }
   
   includedirs 
   { 
    "./"; "../../third_party",
	"%{prj.name}/include"
   }
	
   filter "configurations:Debug"
      defines { "DEBUG;_DEBUG;_WINDOWS;_USRDLL;BASE_EXPORTS" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG;_WINDOWS;_USRDLL;BASE_EXPORTS" }
      optimize "On"
	  
------------------------------------------------------------------------------------------------
project "math"
	location "math"
	kind "SharedLib"
	language "C++"
	cppdialect "C++17"
	targetdir ("bin/" .. outputdir .. "/")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	
   files { "%{prj.name}/**.h", "%{prj.name}/**.cpp"}
   
   includedirs { "./"; "../../third_party" }

   links { "base" }
   
   filter "configurations:Debug"
      defines { "DEBUG;_DEBUG;_WINDOWS;_USRDLL;BASE_EXPORTS" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG;_WINDOWS;_USRDLL;BASE_EXPORTS" }
      optimize "On"
	  
	  --postbuildcommands
	  --{
	  --("{COPY} %{cfg.buidtarget.relpath} ../bin/" .. outputdir .. "/Mgoonga")
	  --}

project "tcp_lib"
	location "tcp_lib"
	kind "SharedLib"
	language "C++"
	cppdialect "C++17"
	targetdir ("bin/" .. outputdir .. "/")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	
   files { "%{prj.name}/**.h", "%{prj.name}/**.cpp"}
   
   includedirs { "../../third_party" }
	
   filter "configurations:Debug"
      defines { "DEBUG;_DEBUG;_WINDOWS;_USRDLL;BASE_EXPORTS" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG;_WINDOWS;_USRDLL;BASE_EXPORTS" }
      optimize "On"
	  
	  --postbuildcommands
	  --{
	  --("{COPY} %{cfg.buidtarget.relpath} ../bin/" .. outputdir .. "/Mgoonga")
	  --}
	  
project "opengl_assets"
	location "opengl_assets"
	kind "SharedLib"
	language "C++"
	cppdialect "C++17"
	targetdir ("bin/" .. outputdir .. "/")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	
   files { "%{prj.name}/**.h", "%{prj.name}/**.cpp"}
   
   includedirs 
   { 
   "./"; "../../third_party",
   "./"; "../../third_party/freetype-2.11.0/include"
   }
	
   libdirs 
   { 
   "../../third_party/libs/Win64",
   "../../third_party/glew-2.1.0/lib/Release/x64",
   "../../third_party/SDL/VisualC/x64/Release/",
   "../../third_party/assimp-3.1.1/build/code/Debug",
   "../../third_party/SDL2_image-2.0.4/VisualC/x64/Debug",
   "../../third_party/IL",
   "../../third_party/freetype-2.11.0/objs"
   }

   links
   {  
   "base", 
   "math",
   "assimpd", 
   "opengl32",
   "glew32",   
   "DevIL", 
   "ILU", 
   "ILUT", 
   "OpenAl32", 
   "SDL2_image", 
   "SDL2", 
   "SDL2main",  
   "freetype.lib",
   }
	
   filter "configurations:Debug"
      defines { "DEBUG;_DEBUG;_WINDOWS;_USRDLL;BASE_EXPORTS" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG;_WINDOWS;_USRDLL;BASE_EXPORTS" }
      optimize "On"
	  
	 --postbuildcommands
	  --{
	  --("{COPY} %{cfg.buidtarget.relpath} ../bin/" .. outputdir .. "/Mgoonga")
	  --}

project "sdl_assets"
	location "sdl_assets"
	kind "SharedLib"
	language "C++"
	cppdialect "C++17"
	targetdir ("bin/" .. outputdir .. "/")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	
   files { "%{prj.name}/**.h", "%{prj.name}/**.cpp"}
   
   includedirs { "./"; "../../third_party" }
	
   libdirs 
   { 
   "../../third_party/libs/Win64",
   "../../third_party/glew-2.1.0/lib/Release/x64",
   "../../third_party/SDL/VisualC/x64/Release/",
   "../../third_party/assimp-3.1.1/build/code/Debug",
   "../../third_party/SDL2_image-2.0.4/VisualC/x64/Debug",
   "../../third_party/IL",
   "../../third_party/freetype-2.11.0/objs"
   }

   links
   {  
   "base", 
   "math",
   "opengl_assets",
   "assimpd", 
   "opengl32",
   "glew32",   
   "DevIL", 
   "ILU", 
   "ILUT", 
   "OpenAl32", 
   "SDL2_image", 
   "SDL2", 
   "SDL2main",
   "freetype.lib",
   }
	
   filter "configurations:Debug"
      defines { "DEBUG;_DEBUG;_WINDOWS;_USRDLL;BASE_EXPORTS" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG;_WINDOWS;_USRDLL;BASE_EXPORTS" }
      optimize "On"
	  
	 --postbuildcommands
	  --{
	  --("{COPY} %{cfg.buidtarget.relpath} ../bin/" .. outputdir .. "/Mgoonga")
	  --}

--------------------------------------------------------------------------------------------------	  
project "game_assets"
	location "game_assets"
	kind "SharedLib"
	language "C++"
	cppdialect "C++17"
	targetdir ("bin/" .. outputdir .. "/")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	
   files { "%{prj.name}/**.h", "%{prj.name}/**.cpp"}
   
   includedirs 
   {
	"./"; "../../third_party",
	"../yaml-cpp/include"   
   }
	
   libdirs 
   { 
   "../../third_party/libs/Win64",
   "../../third_party/glew-2.1.0/lib/Release/x64",
   "../../third_party/SDL/VisualC/x64/Release/",
   "../../third_party/assimp-3.1.1/build/code/Debug",
   "../../third_party/SDL2_image-2.0.4/VisualC/x64/Debug",
   "../../third_party/IL",
   "../../third_party/freetype-2.11.0/objs"
   }

   links
   {  
   "base", 
   "math",
   "yaml-cpp",
   "opengl_assets",
   "sdl_assets",
   "tcp_lib",
   "assimpd", 
   "opengl32",
   "glew32",   
   "DevIL", 
   "ILU", 
   "ILUT", 
   "OpenAl32", 
   "SDL2_image", 
   "SDL2", 
   "SDL2main",
   "freetype.lib",
   }
	
   filter "configurations:Debug"
      defines { "DEBUG;_DEBUG;_WINDOWS;_USRDLL;BASE_EXPORTS" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG;_WINDOWS;_USRDLL;BASE_EXPORTS" }
      optimize "On"

----------------------------------------------------------------------------------------------------	  
project "Mgoonga"
	location "Mgoonga"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	targetdir ("bin/" .. outputdir .. "/")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	
   files { "%{prj.name}/**.h", "%{prj.name}/**.cpp"}
   
   includedirs { "./"; "../../third_party" }
   
   libdirs 
   { 
   "../../third_party/libs/Win64",
   "../../third_party/glew-2.1.0/lib/Release/x64",
   "../../third_party/SDL/VisualC/x64/Release/",
   "../../third_party/assimp-3.1.1/build/code/Debug",
   "../../third_party/SDL2_image-2.0.4/VisualC/x64/Debug",
   "../../third_party/IL",
   "../../third_party/freetype-2.11.0/objs"
   }

   links
   { 
   "base", 
   "math",
   "opengl_assets",
   "sdl_assets",
   "game_assets",
   "assimpd", 
   "opengl32",
   "glew32",   
   "DevIL", 
   "ILU", 
   "ILUT", 
   "OpenAl32", 
   "SDL2_image", 
   "SDL2", 
   "SDL2main",  
   "freetype.lib",
   }
	
   filter "configurations:Debug"
      defines { "DEBUG;_DEBUG;_WINDOWS" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG;_WINDOWS" }
      optimize "On"
	  
project "AmericanTreasureGame"
	location "AmericanTreasureGame"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	targetdir ("bin/" .. outputdir .. "/")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	
   files { "%{prj.name}/**.h", "%{prj.name}/**.cpp"}
   
   includedirs { "./"; "../../third_party" }
   
   libdirs 
   { 
   "../../third_party/libs/Win64",
   "../../third_party/glew-2.1.0/lib/Release/x64",
   "../../third_party/SDL/VisualC/x64/Release/",
   "../../third_party/assimp-3.1.1/build/code/Debug",
   "../../third_party/SDL2_image-2.0.4/VisualC/x64/Debug",
   "../../third_party/IL",
   "../../third_party/freetype-2.11.0/objs"
   }

   links
   { 
   "base", 
   "math",
   "opengl_assets",
   "sdl_assets",
   "game_assets",
   "assimpd", 
   "opengl32",
   "glew32",   
   "DevIL", 
   "ILU", 
   "ILUT", 
   "OpenAl32", 
   "SDL2_image", 
   "SDL2", 
   "SDL2main",  
   "freetype.lib",
   }
	
   filter "configurations:Debug"
      defines { "DEBUG;_DEBUG;_WINDOWS" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG;_WINDOWS" }
      optimize "On"
	  
project "SandBoxGame"
	location "SandBoxGame"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	targetdir ("bin/" .. outputdir .. "/")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	
   files { "%{prj.name}/**.h", "%{prj.name}/**.cpp"}
   
   includedirs { "./"; "../../third_party" }
   
   libdirs 
   { 
   "../../third_party/libs/Win64",
   "../../third_party/glew-2.1.0/lib/Release/x64",
   "../../third_party/SDL/VisualC/x64/Release/",
   "../../third_party/assimp-3.1.1/build/code/Debug",
   "../../third_party/SDL2_image-2.0.4/VisualC/x64/Debug",
   "../../third_party/IL",
   "../../third_party/freetype-2.11.0/objs"
   }

   links
   { 
   "base", 
   "math",
   "opengl_assets",
   "sdl_assets",
   "game_assets",
   "assimpd", 
   "opengl32",
   "glew32",   
   "DevIL", 
   "ILU", 
   "ILUT", 
   "OpenAl32", 
   "SDL2_image", 
   "SDL2", 
   "SDL2main",  
   "freetype.lib",
   }
	
   filter "configurations:Debug"
      defines { "DEBUG;_DEBUG;_WINDOWS" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG;_WINDOWS" }
      optimize "On"
	  