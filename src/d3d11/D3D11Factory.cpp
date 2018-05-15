#include "d3d11/D3D11Factory.h"

D3D11Factory::D3D11Factory() :
	m_renderContext(&m_renderer)
{}

D3D11Factory::~D3D11Factory()
{
	if (!m_bHookMode && m_pSwapChain)
		m_pSwapChain->Release();
}

bool D3D11Factory::Initialize(HWND hWindow)
{
	// Swap Chain Desc
	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));
	scd.BufferCount = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = hWindow;
	scd.SampleDesc.Count = 4;
	scd.Windowed = TRUE;

	// Create Device & Swap Chain
	auto hResult = D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		NULL,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&scd,
		&m_pSwapChain,
		&m_renderer.m_pDevice,
		NULL,
		&m_renderContext.m_pDeviceContext
	);

	// Done
	m_bHookMode = false;
	return SUCCEEDED(hResult);
}

bool D3D11Factory::InitializeFromHook(IDXGISwapChain* pSwapChain)
{
	if (pSwapChain == nullptr)
		return false;
	m_pSwapChain = pSwapChain;

	// Device
	HRESULT hResult = m_pSwapChain->GetDevice(__uuidof(m_renderer.m_pDevice), (void**)&m_renderer.m_pDevice);
	if (FAILED(hResult))
		return false;

	// Device Context
	m_renderer.m_pDevice->GetImmediateContext(&m_renderContext.m_pDeviceContext);

	// Done
	m_bHookMode = true;
	return true;
}

bool D3D11Factory::Present()
{
	if (m_bHookMode)
	{
		// Setup
		if (!m_renderer.m_bIsSetUp && !m_renderer.Setup())
			return false;

		// TODO: Render Queue?
	}
	else
	{
		// TODO: Render Queue?
		m_pSwapChain->Present(0, 0);
	}

	return true;
}

bool renderlib::CreateD3D11Factory(ID3D11Factory** ppFactory)
{
	*ppFactory = new D3D11Factory();
	return true;
}