#include "d3d11/D3D11Renderer.h"
#include "d3d11/D3D11RenderContext.h"
#include "d3d11/D3D11Texture.h"
#include "d3d11/D3D11ShaderBundle.h"
#include "d3d11/D3D11Shaders.h"

D3D11Texture::D3D11Texture(D3D11Renderer* pRenderer) :
    m_pRenderer(pRenderer)
{
    memset(&m_vertices, 0, sizeof(m_vertices));
}

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

    // Texture Shader
    textureDrawInfo.pShaderBundle = new D3D11ShaderBundle(pRenderer);
    textureDrawInfo.pShaderBundle->SetShaders(d3d11_texture_shader, d3d11_texture_shader);
    textureDrawInfo.pShaderBundle->SetupInputLayout(d3d11_texture_input, ARRAYSIZE(d3d11_texture_input));

    if (!textureDrawInfo.pShaderBundle->Initialize())
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

void D3D11Texture::Release()
{
    // Texture
    if (m_pTexture)
	{
        m_pTexture->Release();
        m_pTexture = nullptr;
    }

    // Resource View
    if (m_pResourceView)
	{
        m_pResourceView->Release();
        m_pResourceView = nullptr;
    }

    // Vertex Buffer
    if (m_pVertexBuffer)
	{
        m_pVertexBuffer->Release();
        m_pVertexBuffer = nullptr;
    }
}

void D3D11Texture::Render(IRenderContext* pRenderContext)
{
	auto d3d11RenderContext = reinterpret_cast<D3D11RenderContext*>(pRenderContext);
	auto pDeviceContext = d3d11RenderContext->GetDeviceContext();

    // Vertex Buffer
    if ((m_pVertexBuffer == nullptr || m_bUpdateVertexBuffer) && !UpdateVertexBuffer(pRenderContext))
        return;

    // Shader
	d3d11RenderContext->m_textureDrawInfo.pShaderBundle->Apply(d3d11RenderContext);
	pDeviceContext->PSSetShaderResources(0, 1, &m_pResourceView);

    // Sampler
	pDeviceContext->PSSetSamplers(0, 1, &d3d11RenderContext->m_textureDrawInfo.pSamplerState);

    // Vertex
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    uint32_t stride = sizeof(TextureVertex);
    uint32_t offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

    // Draw
	pDeviceContext->Draw(6, 0);
}

bool D3D11Texture::UpdateVertexBuffer(IRenderContext* pRenderContext)
{
	auto pDeviceContext = reinterpret_cast<D3D11RenderContext*>(pRenderContext)->GetDeviceContext();

    // Viewport Size
    const Vector2f& viewport = pRenderContext->GetViewportSize();

    const float scalex = 1 / viewport.x * 2.f;
    const float scaley = 1 / viewport.y * 2.f;

    // Rect Matrix
    XMFLOAT4A rect(
        static_cast<float>(m_position.x),
        static_cast<float>(m_position.y),
        static_cast<float>(m_size.x),
        static_cast<float>(m_size.y)
    );

    rect.z = (rect.x + rect.z) * scalex - 1.f;
    rect.w = 1.f - (rect.y + rect.w) * scaley;
    rect.x = rect.x * scalex - 1.f;
    rect.y = 1.f - rect.y * scaley;

    // Vertices
    m_vertices[0] = {XMFLOAT3(rect.x, rect.w, 0.5f), XMFLOAT2(0.f, 1.f)};
    m_vertices[1] = {XMFLOAT3(rect.x, rect.y, 0.5f), XMFLOAT2(0.f, 0.f)};
    m_vertices[2] = {XMFLOAT3(rect.z, rect.w, 0.5f), XMFLOAT2(1.f, 1.f)};
    m_vertices[3] = {XMFLOAT3(rect.z, rect.y, 0.5f), XMFLOAT2(1.f, 0.f)};
    m_vertices[4] = {XMFLOAT3(rect.z, rect.w, 0.5f), XMFLOAT2(1.f, 1.f)};
    m_vertices[5] = {XMFLOAT3(rect.x, rect.y, 0.5f), XMFLOAT2(0.f, 0.f)};

    // Create Vertex Buffer
    if (!m_pVertexBuffer)
	{
        D3D11_BUFFER_DESC vertexDesc;
        ZeroMemory(&vertexDesc, sizeof(vertexDesc));
        vertexDesc.Usage = D3D11_USAGE_DYNAMIC;
        vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vertexDesc.ByteWidth = sizeof(TextureVertex) * 6;
        vertexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        D3D11_SUBRESOURCE_DATA resourceData;
        ZeroMemory(&resourceData, sizeof(resourceData));
        resourceData.pSysMem = m_vertices;

        if (FAILED(m_pRenderer->GetDevice()->CreateBuffer(&vertexDesc, &resourceData, &m_pVertexBuffer)))
            return false;
    }

    // Open (Map)
    D3D11_MAPPED_SUBRESOURCE subresource;
    if (FAILED(pDeviceContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource)))
        return false;

    // Copy
    memcpy(subresource.pData, (void*)&m_vertices, sizeof(m_vertices));

    // Close (Unmap)
    pDeviceContext->Unmap(m_pVertexBuffer, 0);

    // Done
	m_bUpdateVertexBuffer = false;
    return true;
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

	if (FAILED(m_pRenderer->GetDevice()->CreateShaderResourceView(pResource, &resourceView, &m_pResourceView)))
	{
		pResource->Release();
		return false;
	}

    // Done
    pResource->Release();
    return true;
}

void D3D11Texture::SetPosition(const Vector2& pos)
{
    if (m_position == pos)
        return;

    m_position = pos;
	m_bUpdateVertexBuffer = true;
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

    // Shader Resource View
    if (!InitializeShaderResourceView())
        return false;

    // Vertex Buffer
    if (!UpdateVertexBuffer(pRenderContext))
        return false;

    // Done
    return true;
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

    // Shader Resource View
    if (!InitializeShaderResourceView())
        return false;

    // Done
    return true;
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
