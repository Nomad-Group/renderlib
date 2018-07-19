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

DXGI_FORMAT TranslateTextureFormatEnum(TextureFormat eFormat)
{
	switch (eFormat)
	{
	case TextureFormat::RGBA_UInt32:
		return DXGI_FORMAT_R8G8B8A8_UNORM;

	case TextureFormat::RGBA_Float32:
		return DXGI_FORMAT_R32G32B32A32_FLOAT;

	case TextureFormat::DepthStencil_Float32_UInt8:
		return DXGI_FORMAT_D24_UNORM_S8_UINT;

	case TextureFormat::Depth_Float32:
		return DXGI_FORMAT_D32_FLOAT;

	default:
		return DXGI_FORMAT_UNKNOWN;
	}
}

UINT TranslateBindFlags(TextureBinding eBinding)
{
	switch (eBinding)
	{
	case TextureBinding::ShaderResource:
		return D3D11_BIND_SHADER_RESOURCE;

	case TextureBinding::RenderTarget:
		return D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;

	case TextureBinding::DepthStencil:
		return D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;

	default:
		return 0;
	}
}

bool D3D11RenderTexture::Create(const math::Vector2& size, TextureFormat eFormat, TextureBinding eBinding, const void* pInitialData, const size_t stPitch)
{
	Release();
	m_size = size;

	// Texture Description
	D3D11_TEXTURE2D_DESC textureDesc;
	memset(&textureDesc, 0, sizeof(D3D11_TEXTURE2D_DESC));

	textureDesc.Width = size.x;
	textureDesc.Height = size.y;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;

	textureDesc.BindFlags = TranslateBindFlags(eBinding);
	textureDesc.Format = TranslateTextureFormatEnum(eFormat);

	// Create (without initial data)
	if (pInitialData == nullptr)
	{
		return
			SUCCEEDED(m_pRenderer->GetDevice()->CreateTexture2D(&textureDesc, nullptr, &m_pTexture)) &&
			InitializeShaderResourceView();
	}

	// Subresource
	D3D11_SUBRESOURCE_DATA subresourceData;
	ZeroMemory(&subresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	subresourceData.pSysMem = pInitialData;
	subresourceData.SysMemPitch = stPitch;

	// Create
	if (FAILED(m_pRenderer->GetDevice()->CreateTexture2D(&textureDesc, &subresourceData, &m_pTexture)))
		return false;

	return InitializeShaderResourceView();
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
	ZeroMemory(&resourceView, sizeof(resourceView));

    resourceView.Format = textureDesc.Format;
    resourceView.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	resourceView.Texture2D.MipLevels = textureDesc.MipLevels;

	// Create
	auto hr = m_pRenderer->GetDevice()->CreateShaderResourceView(pResource, &resourceView, &m_pShaderResourceView);
	return SUCCEEDED(hr);
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

bool D3D11RenderTexture::LoadFromMemory(uint8_t* pImage, uint32_t uiWidth, uint32_t uiHeight, ColorFormat format)
{
	return 	
		Create(math::Vector2(uiWidth, uiHeight), TextureFormat::RGBA_UInt32, TextureBinding::ShaderResource, pImage, uiWidth * 4) &&
		InitializeShaderResourceView();
}

bool D3D11RenderTexture::LoadFromFile(const std::string& path)
{
	int w, h, n;
	unsigned char* pImageData = stbi_load(path.c_str(), &w, &h, &n, 4);
	if (pImageData == nullptr)
		return false;

	const bool success = LoadFromMemory(pImageData, w, h, ColorFormat::RGBA);
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

void D3D11RenderTexture::Bind(IRenderContext* pRenderContext, ShaderType eShaderType, uint8_t uiSlot)
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