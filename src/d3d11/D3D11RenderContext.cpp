#include "d3d11/D3D11RenderContext.h"
#include "d3d11/D3D11BlendState.h"
#include "d3d11/D3D11RenderTarget.h"
#include "d3d11/D3D11Rect.h"
#include "d3d11/D3D11VideoBuffer.h"

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

	// TODO: This has to be configurable
	m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	
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

void D3D11RenderContext::DrawRect(const Rect& rect, const RGBA& color)
{
	if (m_pRect == nullptr)
		return;

	m_pRect->DrawRect(this, rect, color);
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