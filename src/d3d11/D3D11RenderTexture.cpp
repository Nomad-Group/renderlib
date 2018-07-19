#include "D3D11Renderer.h"
#include "D3D11RenderContext.h"
#include "D3D11RenderTexture.h"
#include "D3D11ShaderBundle.h"
#include "D3D11Shaders.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

D3D11RenderTexture::D3D11RenderTexture(D3D11Renderer* pRenderer) :
    m_pRenderer(pRenderer)
{}

D3D11RenderTexture::~D3D11RenderTexture()
{
    Release();
}

void D3D11RenderTexture::Release()
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

bool D3D11RenderTexture::InitializeShaderResourceView()
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

bool D3D11RenderTexture::LoadFrom2DTexture(IRenderContext* pRenderContext, ID3D11Texture2D* pTexture)
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

bool D3D11RenderTexture::LoadFromMemory(IRenderContext* pRenderContext, uint8_t* pImage, uint32_t uiWidth, uint32_t uiHeight, ColorFormat format)
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
            textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            break;

        case ColorFormat::BGRA:
            textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
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

bool D3D11RenderTexture::LoadFromFile(IRenderContext* pRenderContext, const std::string& path)
{
	int w, h, n;
	unsigned char* pImageData = stbi_load(path.c_str(), &w, &h, &n, 4);
	if (pImageData == nullptr)
		return false;

	const bool success = LoadFromMemory(pRenderContext, pImageData, w, h, ColorFormat::RGBA);
	stbi_image_free(pImageData);

	return success;
}

bool D3D11RenderTexture::BlitFromMemory(IRenderContext* pRenderContext, uint8_t* pImage, const uint32_t rowPitch, const Vector2& position,
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
