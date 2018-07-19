#include "D3D11Renderer.h"
#include "D3D11RenderContext.h"
#include "D3D11RenderTarget.h"
#include "D3D11RenderTexture.h"

D3D11RenderTarget::D3D11RenderTarget(D3D11Renderer* pRenderer) :
	m_pRenderer(pRenderer)
{}

D3D11RenderTarget::~D3D11RenderTarget()
{
	if (m_pRenderTargetView)
		m_pRenderTargetView->Release();

	delete m_pTexture;
}

bool D3D11RenderTarget::Initialize(const Vector2& size, TextureFormat eFormat)
{
	auto pDevice = m_pRenderer->GetDevice();

    // Create Texture
	if(m_pTexture == nullptr)
		m_pTexture = new D3D11RenderTexture(m_pRenderer);

	if (!m_pTexture->Create(size, eFormat, TextureBinding::RenderTarget))
		return false;

    // Render Target View Description
    D3D11_RENDER_TARGET_VIEW_DESC viewDesc;
    memset(&viewDesc, 0, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
    viewDesc.Format = TranslateTextureFormatEnum(eFormat);
    viewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

    // Create View
    if (FAILED(pDevice->CreateRenderTargetView(m_pTexture->GetTexture(), &viewDesc, &m_pRenderTargetView)))
        return false;

    // Done
    return true;
}

bool D3D11RenderTarget::InitializeBackbuffer()
{
	ID3D11Texture2D* pBackBuffer = nullptr;
	HRESULT hResult = m_pRenderer->m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));
	if (FAILED(hResult))
		return false;

	hResult = m_pRenderer->m_pDevice->CreateRenderTargetView(pBackBuffer, NULL, &m_pRenderTargetView);
	if (FAILED(hResult))
		return false;

    pBackBuffer->Release();
	return true;
}

void D3D11RenderTarget::Clear(IRenderContext* pRenderContext, const RGBA& col)
{
	const float dcolor[4] = {
		static_cast<float>(col.r) / 255,
		static_cast<float>(col.g) / 255,
		static_cast<float>(col.b) / 255,
		static_cast<float>(col.a) / 255
	};

	auto pDeviceContext = static_cast<D3D11RenderContext*>(pRenderContext)->GetDeviceContext();
    if(m_pRenderTargetView)
	    pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, dcolor);
}

void D3D11RenderTarget::Apply(IRenderContext* pRenderContext)
{
	auto pDeviceContext = static_cast<D3D11RenderContext*>(pRenderContext)->GetDeviceContext();
    pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, nullptr);
}