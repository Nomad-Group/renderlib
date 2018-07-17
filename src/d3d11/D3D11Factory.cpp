#include "d3d11/D3D11Factory.h"
#include "d3d11/D3D11RenderQueue.h"

#pragma comment(lib, "d3d11.lib")

D3D11Factory::D3D11Factory() :
	m_renderContext(&m_renderer)
{}

D3D11Factory::~D3D11Factory()
{
	if (!m_bHookMode && m_pSwapChain)
		m_pSwapChain->Release();

	for (auto pRenderQueue : m_renderQueues)
		delete pRenderQueue;
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

	scd.BufferDesc.RefreshRate.Numerator = 60;
	scd.BufferDesc.RefreshRate.Denominator = 1;

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
	m_renderer.m_pSwapChain = m_pSwapChain;

	// Setup
	if (!m_renderer.Setup() || !m_renderContext.Setup())
		return false;

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

bool D3D11Factory::Render()
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
		// vsync = 1, no vsync = 0
		m_pSwapChain->Present(1, 0);
	}

	return true;
}

IRenderQueue* D3D11Factory::CreateRenderQueue()
{
	m_renderQueues.push_back(new D3D11RenderQueue(&m_renderer));
	return m_renderQueues.back();
}

bool renderlib::CreateD3D11Factory(ID3D11Factory** ppFactory)
{
	*ppFactory = new D3D11Factory();
	return true;
}