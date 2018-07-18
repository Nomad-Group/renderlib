#include "d3d11/D3D11Renderer.h"
#include "d3d11/D3D11RenderContext.h"
#include "d3d11/D3D11Texture.h"
#include "d3d11/D3D11ShaderBundle.h"
#include "d3d11/D3D11Shaders.h"

D3D11Texture::D3D11Texture(D3D11Renderer* pRenderer) :
    m_pRenderer(pRenderer)
{}

D3D11Texture::~D3D11Texture()
{
    Release();
}

bool D3D11Texture::SetupContext(D3D11Renderer* pRenderer, D3D11RenderContext* pContext)
{
	auto& textureDrawInfo = pContext->m_textureDrawInfo;

	// Init once
	if (textureDrawInfo.pShaderBundle)
		return true;

	// Input Layout
	textureDrawInfo.pInputLayout = pRenderer->CreateShaderInputLayout();
	textureDrawInfo.pInputLayout->AddFloat("POSITION", 2);
	textureDrawInfo.pInputLayout->AddFloat("TEXCOORD", 2);

	// Shader Bundle
	textureDrawInfo.pShaderBundle = new D3D11ShaderBundle(pRenderer);
	textureDrawInfo.pShaderBundle->SetShaders(d3d11_texture_shader, d3d11_texture_shader);

	if (!textureDrawInfo.pShaderBundle->Initialize() ||
		!textureDrawInfo.pShaderBundle->SetInputLayout(textureDrawInfo.pInputLayout))
		return false;

	// Vertex Buffer
	textureDrawInfo.pVertexBuffer = pRenderer->CreateBuffer(BufferType::Vertex, textureDrawInfo.pInputLayout->GetSize() * 6, BufferUsage::Dynamic);
	if (!textureDrawInfo.pVertexBuffer->Initialize())
		return false;

    // Sampler State
    D3D11_SAMPLER_DESC colorMapDesc;
    ZeroMemory(&colorMapDesc, sizeof(colorMapDesc));

    colorMapDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    colorMapDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    colorMapDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    colorMapDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    colorMapDesc.MaxAnisotropy = 1;
    colorMapDesc.MipLODBias = 0.0f;
    colorMapDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    colorMapDesc.MaxLOD = D3D11_FLOAT32_MAX;
    colorMapDesc.MinLOD = 0;

    // Sampler State
    if (FAILED(pRenderer->GetDevice()->CreateSamplerState(&colorMapDesc, &textureDrawInfo.pSamplerState)))
        return false;

    // Done
    return true;
}

D3D11RenderContext::TextureDrawInfo::~TextureDrawInfo()
{
	delete pShaderBundle;
	delete pInputLayout;
	delete pVertexBuffer;
	if (pSamplerState)
		pSamplerState->Release();
}

void D3D11RenderContext::DrawTexture(IRenderTexture* pTexture, const math::Vector2& position, const math::Vector2& size)
{
	math::Vector2 renderSize = size;
	if (size.x == 0 && size.y == 0)
		renderSize = pTexture->GetSize();

	// Position
	const math::Vector2f scale(1 / (float)m_size.x * 2.f, 1 / (float)m_size.y * 2.f);
	const math::Rectf rect(
		(float)position.x * scale.x - 1.0f,
		1.0f - (float)position.y * scale.y,
		((float)position.x + (float)renderSize.x) * scale.x - 1.0f,
		1.0f - ((float)position.y + (float)renderSize.y) * scale.y
	);

	const D3D11Texture::TextureVertex vertices[6] = {
		{ rect.x, rect.h, 0.0f, 1.0f },
		{ rect.x, rect.y, 0.0f, 0.0f },
		{ rect.w, rect.h, 1.0f, 1.0f },
		{ rect.w, rect.y, 1.0f, 0.0f },
		{ rect.w, rect.h, 1.0f, 1.0f },
		{ rect.x, rect.y, 0.0f, 0.0f }
	};

	void* pBufferData = nullptr;
	if(!m_textureDrawInfo.pVertexBuffer->Map(this, BufferAccess::WriteDiscard, &pBufferData))
		return;

	memcpy(pBufferData, vertices, sizeof(vertices));
	m_textureDrawInfo.pVertexBuffer->Unmap(this);

	// Shader & Resources
	auto pShaderResourceView = reinterpret_cast<D3D11Texture*>(pTexture)->GetShaderResourceView();
	m_pDeviceContext->PSSetShaderResources(0, 1, &pShaderResourceView);
	m_textureDrawInfo.pShaderBundle->Apply(this);
	m_pDeviceContext->PSSetSamplers(0, 1, &m_textureDrawInfo.pSamplerState);

	// Draw
	m_textureDrawInfo.pVertexBuffer->Apply(this, m_textureDrawInfo.pInputLayout->GetSize());
	m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Draw(6);
}

void D3D11Texture::Release()
{
    // Texture
    if (m_pTexture)
	{
        m_pTexture->Release();
        m_pTexture = nullptr;
    }

    // Shader Resource View
    if (m_pShaderResourceView)
	{
        m_pShaderResourceView->Release();
        m_pShaderResourceView = nullptr;
    }
}

bool D3D11Texture::InitializeShaderResourceView()
{
    // Resource
    ID3D11Resource* pResource = nullptr;
    if (FAILED(m_pTexture->QueryInterface(__uuidof(ID3D11Resource), reinterpret_cast<LPVOID*>(&pResource))))
        return false;

    // Texture Info
    D3D11_TEXTURE2D_DESC textureDesc;
    m_pTexture->GetDesc(&textureDesc);

    // Resource View
    D3D11_SHADER_RESOURCE_VIEW_DESC resourceView;
    resourceView.Format = textureDesc.Format;
    resourceView.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    resourceView.Texture2D.MostDetailedMip = 0;
    resourceView.Texture2D.MipLevels = textureDesc.MipLevels;

	if (FAILED(m_pRenderer->GetDevice()->CreateShaderResourceView(pResource, &resourceView, &m_pShaderResourceView)))
	{
		pResource->Release();
		return false;
	}

    // Done
    pResource->Release();
    return true;
}

bool D3D11Texture::LoadFrom2DTexture(IRenderContext* pRenderContext, ID3D11Texture2D* pTexture)
{
    Release();

    // Description
    D3D11_TEXTURE2D_DESC textureDesc;
    pTexture->GetDesc(&textureDesc);

    // Texture Size
    m_size.x = textureDesc.Width;
    m_size.y = textureDesc.Height;

    // Create Texture
    if (FAILED(m_pRenderer->GetDevice()->CreateTexture2D(&textureDesc, nullptr, &m_pTexture)))
        return false;

    // Copy
	auto pDeviceContext = reinterpret_cast<D3D11RenderContext*>(pRenderContext)->GetDeviceContext();
	pDeviceContext->CopyResource(m_pTexture, pTexture);

    // Done
    return InitializeShaderResourceView();
}

bool D3D11Texture::LoadFromMemory(IRenderContext* pRenderContext, uint8_t* pImage, uint32_t uiWidth, uint32_t uiHeight, ColorFormat format)
{
	Release();

    // Texture Description
    D3D11_TEXTURE2D_DESC textureDesc;
    ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));

    textureDesc.Width = uiWidth;
    textureDesc.Height = uiHeight;
    textureDesc.ArraySize = 1;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    textureDesc.MipLevels = 1;

    // Color Format
    switch (format)
	{
        case ColorFormat::RGBA:
            textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
            break;

        case ColorFormat::BGRA:
            textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
            break;

        default:
            return false;
    }

    // Subresource
    D3D11_SUBRESOURCE_DATA subresourceData;
    ZeroMemory(&subresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
    subresourceData.pSysMem = pImage;
    subresourceData.SysMemPitch = static_cast<uint32_t>(uiWidth * 4);

    // Create Texture
    if (FAILED(m_pRenderer->GetDevice()->CreateTexture2D(&textureDesc, &subresourceData, &m_pTexture)))
        return false;

    // Setup D3D11Texture
    m_size.x = uiWidth;
    m_size.y = uiHeight;

    // Done
	return InitializeShaderResourceView();
}

bool D3D11Texture::BlitFromMemory(IRenderContext* pRenderContext, uint8_t* pImage, const uint32_t rowPitch, const Vector2& position,
                                  const Vector2& size)
{
    D3D11_BOX box;
    box.front = 0;
    box.back = 1;
    box.left = position.x;
    box.right = box.left + size.x;
    box.top = position.y;
    box.bottom = box.top + size.y;

	auto pDeviceContext = reinterpret_cast<D3D11RenderContext*>(pRenderContext)->GetDeviceContext();
	pDeviceContext->UpdateSubresource(m_pTexture, 0, &box, pImage, rowPitch, size.x * size.y * 4);
    return true;
}
