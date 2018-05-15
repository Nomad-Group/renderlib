#include "d3d11/D3D11RenderContext.h"
#include "d3d11/D3D11BlendState.h"
#include "d3d11/D3D11RenderTarget.h"
#include "d3d11/D3D11Rect.h"

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
	m_pRenderTarget = new D3D11RenderTarget(m_pRenderer->m_pDevice, m_pDeviceContext);
	m_pRenderTarget->InitializeBackbuffer(m_pRenderer->m_pSwapChain);

	// Rect
	m_pRect = new D3D11Rect(m_pRenderer);
	if (!m_pRect->Initialize())
		return false;

	// Blend State
	m_pBlendState = new D3D11BlendState();
	if (!m_pBlendState->Initialize(m_pRenderer->m_pDevice))
		return false;

	// Texture Info
	D3D11Texture::SetupContext(m_pRenderer, this);
	
	// Done
	return true;
}

void D3D11RenderContext::Shutdown()
{
	delete m_pBlendState;
	m_pBlendState = nullptr;

	delete m_pRenderTarget;
	m_pRenderTarget = nullptr;

	delete m_pRect;
	m_pRect = nullptr;
}

void D3D11RenderContext::SetViewportSize(const Vector2& size)
{
	m_size = size;

	m_viewport.Width = static_cast<float>(size.x);
	m_viewport.Height = static_cast<float>(size.y);
}

void D3D11RenderContext::Render()
{
	if (m_pDeviceContext)
		return;

	// Render calls
	m_pDeviceContext->RSSetViewports(1, &m_viewport);

	// Setup render state
	m_pBlendState->Apply(this);
	m_pRenderTarget->Apply();
}

void D3D11RenderContext::DrawRect(const Rect& rect, const RGBA& color)
{
	if (m_pRect == nullptr)
		return;

	m_pRect->DrawRect(this, rect, color);
}