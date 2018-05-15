#include "d3d11/D3D11Renderer.h"
#include "d3d11/D3D11Texture.h"
#include "d3d11/D3D11ShaderBundle.h"
#include "d3d11/D3D11Shaders.h"

D3D11Texture::TextureDrawInfo D3D11Texture::m_textureDrawInfo;

D3D11Texture::D3D11Texture(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext) :
    m_pDevice(pDevice),
    m_pDeviceContext(pDeviceContext)
{
    memset(&m_vertices, 0, sizeof(m_vertices));
}

D3D11Texture::~D3D11Texture()
{
    Release();
}

bool D3D11Texture::Initialize(D3D11Renderer* pRenderer)
{
    m_pRenderer = pRenderer;

    // Init once
    if (m_textureDrawInfo.pShaderBundle)
        return true;

    // Texture Shader
    m_textureDrawInfo.pShaderBundle = new D3D11ShaderBundle(m_pRenderer);
    m_textureDrawInfo.pShaderBundle->SetShaders(d3d11_texture_shader, d3d11_texture_shader);
    m_textureDrawInfo.pShaderBundle->SetupInputLayout(d3d11_texture_input, ARRAYSIZE(d3d11_texture_input));

    if (!m_textureDrawInfo.pShaderBundle->Initialize())
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
    if (FAILED(m_pDevice->CreateSamplerState(&colorMapDesc, &m_textureDrawInfo.pSamplerState)))
        return false;

    // Done
    return true;
}

void D3D11Texture::Release()
{
    // Texture
    if (m_pTexture) {
        m_pTexture->Release();
        m_pTexture = nullptr;
    }

    // Resource View
    if (m_pResourceView) {
        m_pResourceView->Release();
        m_pResourceView = nullptr;
    }

    // Vertex Buffer
    if (m_pVertexBuffer) {
        m_pVertexBuffer->Release();
        m_pVertexBuffer = nullptr;
    }
}

void D3D11Texture::Render()
{
    // Vertex Buffer
    if (!m_pVertexBuffer && !UpdateVertexBuffer())
        return;

    // Shader
    m_textureDrawInfo.pShaderBundle->Apply();
    m_pDeviceContext->PSSetShaderResources(0, 1, &m_pResourceView);

    // Sampler
    m_pDeviceContext->PSSetSamplers(0, 1, &m_textureDrawInfo.pSamplerState);

    // Vertex
    m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    uint32_t stride = sizeof(VertexPos);
    uint32_t offset = 0;
    m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

    // Draw
    m_pDeviceContext->Draw(6, 0);
}

bool D3D11Texture::UpdateVertexBuffer()
{
    // Viewport Size
    float w, h;
    m_pRenderer->GetViewportSize(w, h);

    const float scalex = 1 / w * 2.f;
    const float scaley = 1 / h * 2.f;

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
        vertexDesc.ByteWidth = sizeof(VertexPos) * 6;
        vertexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        D3D11_SUBRESOURCE_DATA resourceData;
        ZeroMemory(&resourceData, sizeof(resourceData));
        resourceData.pSysMem = m_vertices;

        if (FAILED(m_pDevice->CreateBuffer(&vertexDesc, &resourceData, &m_pVertexBuffer)))
            return false;
    }

    // Open (Map)
    D3D11_MAPPED_SUBRESOURCE subresource;
    if (FAILED(m_pDeviceContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource)))
        return false;

    // Copy
    memcpy(subresource.pData, (void*)&m_vertices, sizeof(m_vertices));

    // Close (Unmap)
    m_pDeviceContext->Unmap(m_pVertexBuffer, 0);

    // Done
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

    if (FAILED(m_pDevice->CreateShaderResourceView(pResource, &resourceView, &m_pResourceView)))
        return false;

    // Done
    pResource->Release();
    return true;
}

void D3D11Texture::SetPosition(const Vector2& pos)
{
    if (m_position == pos)
        return;
    m_position = pos;

	UpdateVertexBuffer();
}

bool D3D11Texture::LoadFrom2DTexture(ID3D11Texture2D* pTexture)
{
    Release();

    // Description
    D3D11_TEXTURE2D_DESC textureDesc;
    pTexture->GetDesc(&textureDesc);

    // Texture Size
    m_size.x = textureDesc.Width;
    m_size.y = textureDesc.Height;

    // Create Texture
    if (FAILED(m_pDevice->CreateTexture2D(&textureDesc, nullptr, &m_pTexture))) {
        __debugbreak();
        return false;
    }

    // Copy
    m_pDeviceContext->CopyResource(m_pTexture, pTexture);

    // Shader Resource View
    if (!InitializeShaderResourceView()) {
        __debugbreak();
        return false;
    }

    // Vertex Buffer
    if (!UpdateVertexBuffer()) {
        __debugbreak();
        return false;
    }

    // Done
    return true;
}

bool D3D11Texture::LoadFromMemory(uint8_t* pImage, uint32_t uiWidth, uint32_t uiHeight, ColorFormat format)
{
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
    switch (format) {
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
    if (m_pTexture)
        m_pTexture->Release();

    if (FAILED(m_pDevice->CreateTexture2D(&textureDesc, &subresourceData, &m_pTexture))) {
        __debugbreak();
        return false;
    }

    // Setup D3D11Texture
    m_size.x = uiWidth;
    m_size.y = uiHeight;

    // Shader Resource View
    if (!InitializeShaderResourceView())
        return false;

    // Done
    return true;
}

bool D3D11Texture::BlitFromMemory(uint8_t* pImage, const uint32_t rowPitch, const Vector2& position,
                                  const Vector2& size)
{
    D3D11_BOX box;
    box.front = 0;
    box.back = 1;
    box.left = position.x;
    box.right = box.left + size.x;
    box.top = position.y;
    box.bottom = box.top + size.y;

    m_pDeviceContext->UpdateSubresource(m_pTexture, 0, &box, pImage, rowPitch, size.x * size.y * 4);
    return true;
}
