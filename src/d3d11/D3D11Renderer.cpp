#include "d3d11/D3D11Renderer.h"
#include "d3d11/D3D11RenderContext.h"
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
	}
}

void D3D11Renderer::Shutdown()
{
    // Resources
    for (auto pFont : m_fonts)
        delete pFont;
    m_fonts.clear();

    // FW1FontWrapper
    if (m_pFW1Factory)
        m_pFW1Factory->Release();

    m_pFW1Factory = nullptr;
}

bool D3D11Renderer::Setup()
{
    // FW1FontWrapper
    if (!m_pFW1Factory && FAILED(FW1CreateFactory(FW1_VERSION, &m_pFW1Factory)))
        return false;

    // Done
	m_bIsSetUp = true;
    return true;
}

void D3D11Renderer::Present()
{
	m_pSwapChain->Present(0, 0);
}

void D3D11Renderer::Reset()
{
	//m_pRenderContext->Shutdown();

    // Setup again
    m_bIsSetUp = false;
}

IRenderFont* D3D11Renderer::GetFont(const std::string& name)
{
    // Find Font
    for (auto pFont : m_fonts)
	{
        if (pFont->GetName() == name)
            return pFont;
    }

    // Create new Font
    auto pFont = new D3D11Font(this, name);
    if (!pFont->Initialize())
	{
        delete pFont;
        return nullptr;
    }

    // Store Font & Done
    m_fonts.push_back(pFont);
    return pFont;
}

IRenderTexture* D3D11Renderer::CreateTexture()
{
    return new D3D11Texture(this);
}

IRenderTarget* D3D11Renderer::CreateRenderTarget(const Vector2& size)
{
	auto pRenderTarget = new D3D11RenderTarget();
	if (!pRenderTarget->Initialize(this, size))
	{
		delete pRenderTarget;
		return nullptr;
	}

	return pRenderTarget;
}