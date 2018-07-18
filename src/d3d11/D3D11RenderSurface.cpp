#include "D3D11RenderSurface.h"
#include "D3D11Shaders.h"
#include "D3D11ShaderBundle.h"
#include "D3D11RenderContext.h"
#include "D3D11Texture.h"

D3D11RenderSurface::D3D11RenderSurface(D3D11Renderer* pRenderer, D3D11RenderContext* pRenderContext) :
	m_pRenderer(pRenderer),
	m_pRenderContext(pRenderContext)
{}

bool D3D11RenderSurface::Setup()
{
	return
		SetupRectangle() &&
		SetupTexture();
}

bool D3D11RenderSurface::SetupRectangle()
{
	// Input Layout
	m_rectangleDrawInfo.pInputLayout = m_pRenderer->CreateShaderInputLayout();
	m_rectangleDrawInfo.pInputLayout->AddFloat("POSITION", 2);

	// Shader Bundle
	m_rectangleDrawInfo.pShaderBundle = new D3D11ShaderBundle(m_pRenderer);
	m_rectangleDrawInfo.pShaderBundle->SetShaders(d3d11_rect_shader, d3d11_rect_shader);

	if (!m_rectangleDrawInfo.pShaderBundle->Initialize() ||
		!m_rectangleDrawInfo.pShaderBundle->SetInputLayout(m_rectangleDrawInfo.pInputLayout))
		return false;

	// Vertex Buffer
	m_rectangleDrawInfo.pVertexBuffer = m_pRenderer->CreateBuffer(BufferType::Vertex, m_rectangleDrawInfo.pInputLayout->GetSize() * 6, BufferUsage::Dynamic);
	if (!m_rectangleDrawInfo.pVertexBuffer->Initialize())
		return false;

	// Color Buffer
	m_rectangleDrawInfo.pColorBuffer = m_pRenderer->CreateBuffer(BufferType::Constant, sizeof(float) * 4, BufferUsage::Dynamic);
	if (!m_rectangleDrawInfo.pColorBuffer->Initialize())
		return false;

	// Done
	return true;
}

bool D3D11RenderSurface::SetupTexture()
{
	// Input Layout
	m_textureDrawInfo.pInputLayout = m_pRenderer->CreateShaderInputLayout();
	m_textureDrawInfo.pInputLayout->AddFloat("POSITION", 2);
	m_textureDrawInfo.pInputLayout->AddFloat("TEXCOORD", 2);

	// Shader Bundle
	m_textureDrawInfo.pShaderBundle = new D3D11ShaderBundle(m_pRenderer);
	m_textureDrawInfo.pShaderBundle->SetShaders(d3d11_texture_shader, d3d11_texture_shader);

	if (!m_textureDrawInfo.pShaderBundle->Initialize() ||
		!m_textureDrawInfo.pShaderBundle->SetInputLayout(m_textureDrawInfo.pInputLayout))
		return false;

	// Vertex Buffer
	m_textureDrawInfo.pVertexBuffer = m_pRenderer->CreateBuffer(BufferType::Vertex, m_textureDrawInfo.pInputLayout->GetSize() * 6, BufferUsage::Dynamic);
	if (!m_textureDrawInfo.pVertexBuffer->Initialize())
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
	if (FAILED(m_pRenderer->GetDevice()->CreateSamplerState(&colorMapDesc, &m_textureDrawInfo.pSamplerState)))
		return false;

	// Done
	return true;
}

D3D11RenderSurface::RectangleDrawInfo::~RectangleDrawInfo()
{
	delete pShaderBundle;
	delete pInputLayout;
	delete pVertexBuffer;
	delete pColorBuffer;
}

void D3D11RenderSurface::DrawRect(const Rect& rect, const RGBA& color)
{
	// Update Vertex Buffer
	// Position
	const math::Vector2 viewportSize = m_pRenderContext->GetViewportSize();
	const math::Vector2f scale(1 / (float)viewportSize.x * 2.f, 1 / (float)viewportSize.y * 2.f);
	const math::Rectf frect(
		(float)rect.x * scale.x - 1.0f,
		1.0f - (float)rect.y * scale.y,
		((float)rect.x + (float)rect.w) * scale.x - 1.0f,
		1.0f - ((float)rect.y + (float)rect.h) * scale.y
	);

	const float vertices[12] = {
		frect.x, frect.h,
		frect.x, frect.y,
		frect.w, frect.h,
		frect.w, frect.y,
		frect.w, frect.h,
		frect.x, frect.y
	};

	void* pBufferData = nullptr;
	if (!m_rectangleDrawInfo.pVertexBuffer->Map(m_pRenderContext, BufferAccess::WriteDiscard, &pBufferData))
		return;

	memcpy(pBufferData, vertices, sizeof(vertices));
	m_rectangleDrawInfo.pVertexBuffer->Unmap(m_pRenderContext);

	// Color Buffer
	if (!m_rectangleDrawInfo.pColorBuffer->Map(m_pRenderContext, BufferAccess::WriteDiscard, &pBufferData))
		return;

	float colors[4] = { (float)color.r / 255.0f, (float)color.g / 255.0f, (float)color.b / 255.0f, (float)color.a / 255.0f };
	memcpy(pBufferData, colors, sizeof(colors));
	m_rectangleDrawInfo.pColorBuffer->Unmap(m_pRenderContext);

	// Draw
	m_rectangleDrawInfo.pShaderBundle->Apply(m_pRenderContext);
	m_rectangleDrawInfo.pVertexBuffer->Apply(m_pRenderContext, m_rectangleDrawInfo.pInputLayout->GetSize());
	m_rectangleDrawInfo.pColorBuffer->Apply(m_pRenderContext);
	
	m_pRenderContext->Draw(PrimitiveTopology::TriangleList, 6);
}

D3D11RenderSurface::TextureDrawInfo::~TextureDrawInfo()
{
	delete pShaderBundle;
	delete pInputLayout;
	delete pVertexBuffer;
	if (pSamplerState)
		pSamplerState->Release();
}

void D3D11RenderSurface::DrawTexture(IRenderTexture* pTexture, const math::Vector2& position, const math::Vector2& size)
{
	math::Vector2 renderSize = size;
	if (size.x == 0 && size.y == 0)
		renderSize = pTexture->GetSize();

	// Position
	const math::Vector2 viewportSize = m_pRenderContext->GetViewportSize();
	const math::Vector2f scale(1 / (float)viewportSize.x * 2.f, 1 / (float)viewportSize.y * 2.f);
	const math::Rectf rect(
		(float)position.x * scale.x - 1.0f,
		1.0f - (float)position.y * scale.y,
		((float)position.x + (float)renderSize.x) * scale.x - 1.0f,
		1.0f - ((float)position.y + (float)renderSize.y) * scale.y
	);

	const TextureVertex vertices[6] = {
		{ rect.x, rect.h, 0.0f, 1.0f },
		{ rect.x, rect.y, 0.0f, 0.0f },
		{ rect.w, rect.h, 1.0f, 1.0f },
		{ rect.w, rect.y, 1.0f, 0.0f },
		{ rect.w, rect.h, 1.0f, 1.0f },
		{ rect.x, rect.y, 0.0f, 0.0f }
	};

	void* pBufferData = nullptr;
	if (!m_textureDrawInfo.pVertexBuffer->Map(m_pRenderContext, BufferAccess::WriteDiscard, &pBufferData))
		return;

	memcpy(pBufferData, vertices, sizeof(vertices));
	m_textureDrawInfo.pVertexBuffer->Unmap(m_pRenderContext);

	// Shader & Resources
	auto pDeviceContext = m_pRenderContext->GetDeviceContext();

	auto pShaderResourceView = reinterpret_cast<D3D11Texture*>(pTexture)->GetShaderResourceView();
	pDeviceContext->PSSetShaderResources(0, 1, &pShaderResourceView);
	m_textureDrawInfo.pShaderBundle->Apply(m_pRenderContext);
	pDeviceContext->PSSetSamplers(0, 1, &m_textureDrawInfo.pSamplerState);

	// Draw
	m_textureDrawInfo.pVertexBuffer->Apply(m_pRenderContext, m_textureDrawInfo.pInputLayout->GetSize());
	
	m_pRenderContext->Draw(PrimitiveTopology::TriangleList, 6);
}