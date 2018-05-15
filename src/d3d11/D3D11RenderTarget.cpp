#include "d3d11/D3D11Renderer.h"
#include "d3d11/D3D11RenderTarget.h"
#include "d3d11/D3D11Texture.h"

D3D11RenderTarget::D3D11RenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext) :
	m_pDevice(pDevice),
	m_pDeviceContext(pDeviceContext)
{}

D3D11RenderTarget::~D3D11RenderTarget()
{
	if (m_pRenderTargetView)
		m_pRenderTargetView->Release();
	if (m_pRenderTexture)
		m_pRenderTexture->Release();
}

bool D3D11RenderTarget::Initialize(IRenderer* pRenderer, const Vector2& size)
{
    // Description
    D3D11_TEXTURE2D_DESC textureDesc;
    memset(&textureDesc, 0, sizeof(D3D11_TEXTURE2D_DESC));

	textureDesc.Width = size.x;
	textureDesc.Height = size.y;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // DXGI_FORMAT_R32G32B32A32_FLOAT
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

    // Create Texture
    if (FAILED(m_pDevice->CreateTexture2D(&textureDesc, nullptr, &m_pRenderTexture)))
        return false;

    // Render Target View Description
    D3D11_RENDER_TARGET_VIEW_DESC viewDesc;
    memset(&viewDesc, 0, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));

    viewDesc.Format = textureDesc.Format;
    viewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

    // Create View
    if (FAILED(m_pDevice->CreateRenderTargetView(m_pRenderTexture, &viewDesc, &m_pRenderTargetView))) {
        return false;
    }

    // Shader View
    D3D11_SHADER_RESOURCE_VIEW_DESC shaderDesc;
    memset(&shaderDesc, 0, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

    shaderDesc.Format = textureDesc.Format;
    shaderDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderDesc.Texture2D.MipLevels = 1;

    // Create Shader Resource View
    if (FAILED(m_pDevice->CreateShaderResourceView(m_pRenderTexture, &shaderDesc, &m_pShaderResourceView)))
        return false;

    // Done
    return true;
}

bool D3D11RenderTarget::InitializeBackbuffer(IDXGISwapChain* pSwapChain)
{
	ID3D11Texture2D* pBackBuffer = nullptr;
	HRESULT hResult = pSwapChain->GetBuffer(
		0,
		__uuidof(ID3D11Texture2D),
		reinterpret_cast<void**>(&pBackBuffer)
	);

	if (FAILED(hResult))
		return false;

	hResult = m_pDevice->CreateRenderTargetView(pBackBuffer, NULL, &m_pRenderTargetView);
	if (FAILED(hResult))
		return false;

    pBackBuffer->Release();
	return true;
}

void D3D11RenderTarget::Clear(const RGBA& col)
{
	float dcolor[4];
	dcolor[0] = static_cast<float>(col.r) / 255;
	dcolor[1] = static_cast<float>(col.g) / 255;
	dcolor[2] = static_cast<float>(col.b) / 255;
	dcolor[3] = static_cast<float>(col.a) / 255;

    if(m_pRenderTargetView)
	    m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, dcolor);

	if(m_pDepthStencilView)
	    m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void D3D11RenderTarget::ClearStencil()
{
    if(m_pDepthStencilView)
        m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void D3D11RenderTarget::Apply()
{
    m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, NULL);
}

bool D3D11RenderTarget::CopyTexture(IRenderContext* pRenderContext, IRenderTexture* texture)
{
    auto pTexture = reinterpret_cast<D3D11Texture*>(texture);
	if (pTexture == nullptr || m_bBackbuffer)
		return false;

	return pTexture->LoadFrom2DTexture(pRenderContext, m_pRenderTexture);
}
