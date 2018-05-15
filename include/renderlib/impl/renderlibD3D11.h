#pragma once
#include "../renderlib.h"
#include "../detail/renderlibFactory.h"
#include <Windows.h>
#include <d3d11.h>

namespace renderlib
{
	struct ID3D11Factory : IFactory
	{
		// Create Swap Chain & Device
		virtual bool Initialize(HWND hWindow) = 0;

		// Use renderlib from Hook
		virtual bool InitializeFromHook(IDXGISwapChain*) = 0;

		// Present
		// Hook mode = call from your Present hook, run render calls AFTER!
		// Create mode = calls Present, run render calls BEFORE
		virtual bool Present() = 0;
	};

	__declspec(dllexport) bool CreateD3D11Factory(ID3D11Factory** ppFactory);
}