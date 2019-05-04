project "ImGui"
	kind "StaticLib"
	toolset "v140"
	location "vendor/imgui"
	language "C++"

    targetdir ("bin/" .. outputdir .. "/%{cfg.architecture}")
    objdir ("bin-int/" .. outputdir .. "/%{cfg.architecture}")

	includedirs
	{
		"vendor/glew/include/",
		"vendor/imgui/",
		"vendor/SDL2/include/"
	}

	files
	{
		"vendor/imgui/imconfig.h",
		"vendor/imgui/imgui.h",
		"vendor/imgui/imgui.cpp",
		"vendor/imgui/imgui_draw.cpp",
		"vendor/imgui/imgui_internal.h",
		"vendor/imgui/imgui_widgets.cpp",
		"vendor/imgui/imgui_rectpack.h",
		"vendor/imgui/imgui_textedit.h",
		"vendor/imgui/imgui_truetype.h",
		"vendor/imgui/imgui_demo.cpp",
		"vendor/imgui/examples/imgui_impl_opengl3.cpp",
		"vendor/imgui/examples/imgui_impl_opengl3.h",
		"vendor/imgui/examples/imgui_impl_sdl.cpp",
		"vendor/imgui/examples/imgui_impl_sdl.h"
	}

	defines "IMGUI_IMPL_OPENGL_LOADER_GLEW"	

	filter "system:windows"
		systemversion "latest"
		cppdialect "C++17"
		staticruntime "On"

	filter {"system:windows", "configurations:Release"}
		buildoptions "/MT"
