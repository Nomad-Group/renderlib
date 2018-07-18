#include "d3d11/D3D11RenderContext.h"
#include "d3d11/D3D11BlendState.h"
#include "d3d11/D3D11RenderTarget.h"
#include "d3d11/D3D11VideoBuffer.h"
#include "d3d11/D3D11ShaderBundle.h"
#include "d3d11/D3D11Shaders.h"

D3D11RenderContext::D3D11RenderContext(D3D11Renderer* pRenderer) :
	m_pRenderer(pRenderer)
{}

D3D11RenderContext::~D3D11RenderContext()
{
	if (m_pRenderer->m_bReleaseResources && m_pDeviceContext)
		m_pDeviceContext->Release();
}

bool D3D11RenderContext::Setup()
{
	// Render Target
	m_pRenderTarget = new D3D11RenderTarget();
	m_pRenderTarget->InitializeBackbuffer(m_pRenderer);

	// Blend State
	m_pBlendState = new D3D11BlendState();
	if (!m_pBlendState->Initialize(m_pRenderer->m_pDevice))
		return false;

	// Texture Info
	if (!D3D11Texture::SetupContext(m_pRenderer, this))
		return false;

	// TODO: This has to be configurable
	m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	// TODO: Also configurable
	D3D11_RASTERIZER_DESC rd;
	rd.FillMode = D3D11_FILL_SOLID;
	rd.CullMode = D3D11_CULL_BACK;
	rd.FrontCounterClockwise = FALSE;
	rd.DepthClipEnable = TRUE;
	rd.ScissorEnable = FALSE;
	rd.AntialiasedLineEnable = FALSE;
	rd.MultisampleEnable = FALSE;
	rd.DepthBias = 0;
	rd.DepthBiasClamp = 0.0f;
	rd.SlopeScaledDepthBias = 0.0f;

	//rd.FillMode = D3D11_FILL_WIREFRAME;
	//rd.AntialiasedLineEnable = TRUE;

	ID3D11RasterizerState* pRS = nullptr;
	m_pRenderer->m_pDevice->CreateRasterizerState(&rd, &pRS);
	m_pDeviceContext->RSSetState(pRS);

	// Rectangle
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

void D3D11RenderContext::Shutdown()
{
	delete m_pBlendState;
	m_pBlendState = nullptr;

	delete m_pRenderTarget;
	m_pRenderTarget = nullptr;
}

void D3D11RenderContext::SetViewportSize(const Vector2& size)
{
	m_size = size;

	m_viewport.Width = static_cast<float>(size.x);
	m_viewport.Height = static_cast<float>(size.y);
}

void D3D11RenderContext::Render()
{
	if (m_pDeviceContext == nullptr)
		return;

	// Setup render state
	m_pRenderTarget->Apply(this);
	m_pDeviceContext->RSSetViewports(1, &m_viewport);

	if(m_pBlendState)
		m_pBlendState->Apply(this);
}

void D3D11RenderContext::Draw(size_t stNumElements)
{
	m_pDeviceContext->Draw(stNumElements, 0);
}
void D3D11RenderContext::DrawIndexed(size_t stNumElements)
{
	m_pDeviceContext->DrawIndexed(stNumElements, 0, 0);
}

D3D11RenderContext::RectangleDrawInfo::~RectangleDrawInfo()
{
	delete pShaderBundle;
	delete pInputLayout;
	delete pVertexBuffer;
	delete pColorBuffer;
}

void D3D11RenderContext::DrawRect(const Rect& rect, const RGBA& color)
{
	// Update Vertex Buffer
	// Position
	const math::Vector2f scale(1 / (float)m_size.x * 2.f, 1 / (float)m_size.y * 2.f);
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
	if (!m_rectangleDrawInfo.pVertexBuffer->Map(this, BufferAccess::WriteDiscard, &pBufferData))
		return;

	memcpy(pBufferData, vertices, sizeof(vertices));
	m_rectangleDrawInfo.pVertexBuffer->Unmap(this);

	// Color Buffer
	if (!m_rectangleDrawInfo.pColorBuffer->Map(this, BufferAccess::WriteDiscard, &pBufferData))
		return;

	float colors[4] = { (float)color.r / 255.0f, (float)color.g / 255.0f, (float)color.b / 255.0f, (float)color.a / 255.0f };
	memcpy(pBufferData, colors, sizeof(colors));
	m_rectangleDrawInfo.pColorBuffer->Unmap(this);

	// Draw
	m_rectangleDrawInfo.pShaderBundle->Apply(this);
	m_rectangleDrawInfo.pVertexBuffer->Apply(this, m_rectangleDrawInfo.pInputLayout->GetSize());
	m_rectangleDrawInfo.pColorBuffer->Apply(this);
	Draw(6);
}

void D3D11RenderContext::SaveState()
{
	m_stateSaver.saveCurrentState(m_pDeviceContext);
}
void D3D11RenderContext::RestoreState()
{
	m_stateSaver.restoreSavedState();
}

IRenderTarget* D3D11RenderContext::GetBackBufferRenderTarget() { return m_pRenderTarget; }