#pragma once

namespace dbb {

	class  __declspec(dllexport) NetWork
	{
	public:
		static bool Initialize();
		static void Shutdown();
	};
}

