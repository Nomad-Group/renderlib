#include "d3d11/D3D11Renderer.h"
#include "d3d11/D3D11BlendState.h"
#include "d3d11/D3D11ShaderBundle.h"
#include "d3d11/D3D11RenderTarget.h"
#include "d3d11/D3D11Rect.h"
#include "d3d11/D3D11Font.h"
#include "d3d11/D3D11Texture.h"
#include "FW1FontWrapper/FW1FontWrapper.h"
#include "FW1FontWrapper/CFW1StateSaver.h"

D3D11Renderer::~D3D11Renderer()
{
	if (m_bReleaseResources)
	{
		if(m_pSwapChain)
			m_pSwapChain->Release();

		if(m_pDevice)
			m_pDevice->Release();

		if(m_pDeviceContext)
			m_pDeviceContext->Release();
	}
}

void D3D11Renderer::Shutdown()
{
    if (!m_pDevice || !m_pDeviceContext)
        return;

    // D3D11
    delete m_pBlendState;
    m_pBlendState = nullptr;

    delete m_pRenderTarget;
    m_pRenderTarget = nullptr;

    delete m_pRect;
    m_pRect = nullptr;

    // Resources
    for (auto pFont : m_vFonts)
        delete pFont;
    m_vFonts.clear();

    for (auto& info : m_mTextures)
        delete info.second;
    m_mTextures.clear();

    // FW1FontWrapper
    if (m_pFW1Factory)
        m_pFW1Factory->Release();

    m_pFW1Factory = nullptr;
}

void D3D11Renderer::Render(IDXGISwapChain* pSwapChain)
{
    // Device & Device Context
    if (pSwapChain != nullptr && pSwapChain != m_pSwapChain) {
        m_pSwapChain = pSwapChain;
        m_pSwapChain->GetDevice(__uuidof(m_pDevice), (void**)&m_pDevice);
        m_pDevice->GetImmediateContext(&m_pDeviceContext);
    }

    // State Saver
    //m_stateSaver.saveCurrentState(m_pDeviceContext);

    // Setup
    if (!m_bIsSetUp) {
        if (!Setup()) {
            MessageBoxA(nullptr, "Failed to setup D3D11Renderer!", "ScriptHook", MB_ICONERROR | MB_OK);
            exit(0);
        }

        m_bIsSetUp = true;
    }

    // Render calls
    m_pDeviceContext->RSSetViewports(1, &m_viewport);

    // Setup render state
    m_pBlendState->Apply();
    m_pRenderTarget->Apply();
}

void D3D11Renderer::RestoreState()
{
	//m_stateSaver.restoreSavedState();
}

bool D3D11Renderer::Setup()
{
    // Rect
    m_pRect = new D3D11Rect(this);
    if (!m_pRect->Initialize()) {
        __debugbreak();
        return false;
    }

    // Blend State
    m_pBlendState = new D3D11BlendState();
    if (!m_pBlendState->Initialize(m_pDevice, m_pDeviceContext)) {
        __debugbreak();
        return false;
    }

    // Render Target
    m_pRenderTarget = new D3D11RenderTarget(m_pDevice, m_pDeviceContext);
    m_pRenderTarget->InitializeBackbuffer(m_pSwapChain);

    // FW1FontWrapper
    if (!m_pFW1Factory && FAILED(FW1CreateFactory(FW1_VERSION, &m_pFW1Factory))) {
        __debugbreak();
        return false;
    }

    // Done
    return true;
}

bool D3D11Renderer::SetupNewSwapChain(HWND hWindow)
{
	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));
	scd.BufferCount = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = hWindow;
	scd.SampleDesc.Count = 4;
	scd.Windowed = TRUE;

	auto hResult = D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		NULL,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&scd,
		&m_pSwapChain,
		&m_pDevice,
		NULL,
		&m_pDeviceContext);

	m_bReleaseResources = true;
	if (FAILED(hResult))
		return false;

	return true;
}

void D3D11Renderer::Present()
{
	m_pSwapChain->Present(0, 0);
}

void D3D11Renderer::Reset()
{
    // D3D11
    delete m_pBlendState;
    m_pBlendState = nullptr;

    delete m_pRenderTarget;
    m_pRenderTarget = nullptr;

    delete m_pRect;
    m_pRect = nullptr;

    // Setup again
    m_bIsSetUp = false;
}

IRenderFont* D3D11Renderer::GetFont(const std::string& name)
{
    // Find Font
    for (auto pFont : m_vFonts) {
        if (pFont->GetName() == name)
            return pFont;
    }

    // Create new Font
    auto pFont = new D3D11Font(this, name);
    if (!pFont->Initialize()) {
        delete pFont;
        return nullptr;
    }

    // Store Font & Done
    m_vFonts.push_back(pFont);
    return pFont;
}

IRenderTexture* D3D11Renderer::GetTexture(const std::string& path)
{
    // Find Texture
    for (auto& entry : m_mTextures) {
        if (entry.first == path)
            return entry.second;
    }

    // Create Texture
    auto pTexture = new D3D11Texture(m_pDevice, m_pDeviceContext);
    if (!pTexture->Initialize(this) || !pTexture->LoadFromPNG(path)) {
        delete pTexture;
        return nullptr;
    }

    // Store & Done
    m_mTextures[path] = pTexture;
    return pTexture;
}

IRenderTexture* D3D11Renderer::CreateTexture()
{
    auto pTexture = new D3D11Texture(m_pDevice, m_pDeviceContext);
    if (!pTexture->Initialize(this)) {
        delete pTexture;
        return nullptr;
    }

    return pTexture;
}

IRenderTarget* D3D11Renderer::CreateRenderTarget(const Vector2& size)
{
	auto pRenderTarget = new D3D11RenderTarget(m_pDevice, m_pDeviceContext);
	if (!pRenderTarget->Initialize(this, size))
	{
		delete pRenderTarget;
		return nullptr;
	}

	return pRenderTarget;
}

void D3D11Renderer::SetViewportSize(const Vector2& size)
{
    m_size = size;

    m_viewport.Width  = static_cast<float>(size.x);
    m_viewport.Height = static_cast<float>(size.y);
}

void D3D11Renderer::GetViewportSize(float& outWidth, float& outHeight) const
{
    outWidth  = m_viewport.Width;
    outHeight = m_viewport.Height;
}

IRenderTarget* D3D11Renderer::GetBackBufferRenderTarget() { return m_pRenderTarget; };
