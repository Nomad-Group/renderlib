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

	if (m_pRenderTexture)
		m_pRenderTexture->Release();
}

bool D3D11RenderTarget::Initialize(const Vector2& size, RenderTargetFormat eFormat)
{
	auto pDevice = m_pRenderer->GetDevice();

    // Description
    D3D11_TEXTURE2D_DESC textureDesc;
    memset(&textureDesc, 0, sizeof(D3D11_TEXTURE2D_DESC));

	textureDesc.Width = size.x;
	textureDesc.Height = size.y;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	// Texture Format
	switch (eFormat)
	{
	case RenderTargetFormat::RGBA_UInt32:
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		break;

	case RenderTargetFormat::RGBA_Float:
		textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		break;

	default:
		return false;
	}

    // Create Texture
    if (FAILED(pDevice->CreateTexture2D(&textureDesc, nullptr, &m_pRenderTexture)))
        return false;

    // Render Target View Description
    D3D11_RENDER_TARGET_VIEW_DESC viewDesc;
    memset(&viewDesc, 0, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));

    viewDesc.Format = textureDesc.Format;
    viewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

    // Create View
    if (FAILED(pDevice->CreateRenderTargetView(m_pRenderTexture, &viewDesc, &m_pRenderTargetView)))
        return false;

    // Shader View
    D3D11_SHADER_RESOURCE_VIEW_DESC shaderDesc;
    memset(&shaderDesc, 0, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

    shaderDesc.Format = textureDesc.Format;
    shaderDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderDesc.Texture2D.MipLevels = 1;

    // Create Shader Resource View
    if (FAILED(pDevice->CreateShaderResourceView(m_pRenderTexture, &shaderDesc, &m_pShaderResourceView)))
        return false;

    // Done
    return true;
}

bool D3D11RenderTarget::InitializeDepthStencil(const Vector2& size, RenderTargetFormat eFormat)
{
	auto pDevice = m_pRenderer->GetDevice();

	// Description
	D3D11_TEXTURE2D_DESC textureDesc;
	memset(&textureDesc, 0, sizeof(D3D11_TEXTURE2D_DESC));

	textureDesc.Width = size.x;
	textureDesc.Height = size.y;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	// Texture Format
	switch (eFormat)
	{
	case RenderTargetFormat::DephStencil_Z328S:
		textureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		break;

	default:
		return false;
	}

	// Create Texture
	if (FAILED(pDevice->CreateTexture2D(&textureDesc, nullptr, &m_pRenderTexture)))
		return false;

	// Depth Stencil View
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	depthStencilViewDesc.Format = textureDesc.Format;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	if (FAILED(pDevice->CreateDepthStencilView(m_pRenderTexture, &depthStencilViewDesc, &m_pDepthStencilView)))
		return false;

	// Done
	return true;
}

bool D3D11RenderTarget::InitializeBackbuffer()
{
	ID3D11Texture2D* pBackBuffer = nullptr;
	HRESULT hResult = m_pRenderer->m_pSwapChain->GetBuffer(
		0,
		__uuidof(ID3D11Texture2D),
		reinterpret_cast<void**>(&pBackBuffer)
	);

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
	float dcolor[4];
	dcolor[0] = static_cast<float>(col.r) / 255;
	dcolor[1] = static_cast<float>(col.g) / 255;
	dcolor[2] = static_cast<float>(col.b) / 255;
	dcolor[3] = static_cast<float>(col.a) / 255;

	auto pDeviceContext = static_cast<D3D11RenderContext*>(pRenderContext)->GetDeviceContext();
    if(m_pRenderTargetView)
	    pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, dcolor);

	if(m_pDepthStencilView)
	    pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void D3D11RenderTarget::ClearStencil(IRenderContext* pRenderContext)
{
	auto pDeviceContext = static_cast<D3D11RenderContext*>(pRenderContext)->GetDeviceContext();
    if(m_pDepthStencilView)
        pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, /* D3D11_CLEAR_DEPTH | */ D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void D3D11RenderTarget::Apply(IRenderContext* pRenderContext)
{
	auto pDeviceContext = static_cast<D3D11RenderContext*>(pRenderContext)->GetDeviceContext();
    pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, nullptr);
}

void D3D11RenderTarget::Bind(IRenderContext* pRenderContext, ShaderType eShaderType, uint8_t uiSlot)
{
	auto pDeviceContext = reinterpret_cast<D3D11RenderContext*>(pRenderContext)->GetDeviceContext();

	switch (eShaderType)
	{
	case ShaderType::Vertex:
		pDeviceContext->VSSetShaderResources(uiSlot, 1, &m_pShaderResourceView);
		break;

	case ShaderType::Pixel:
		pDeviceContext->PSSetShaderResources(uiSlot, 1, &m_pShaderResourceView);
		break;

	default:
		return;
	}
}

bool D3D11RenderTarget::CopyTexture(IRenderContext* pRenderContext, IRenderTexture* texture)
{
    auto pTexture = reinterpret_cast<D3D11RenderTexture*>(texture);
	if (pTexture == nullptr || m_bBackbuffer)
		return false;

	return pTexture->LoadFrom2DTexture(pRenderContext, m_pRenderTexture);
}
