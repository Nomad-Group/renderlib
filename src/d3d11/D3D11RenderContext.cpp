#include "D3D11RenderContext.h"
#include "D3D11BlendState.h"
#include "D3D11RenderTarget.h"

D3D11RenderContext::D3D11RenderContext(D3D11Renderer* pRenderer) :
	m_pRenderer(pRenderer),
	m_surface(pRenderer, this)
{
	for (size_t i = 0; i < MAX_RENDER_TARGET_VIEWS; i++)
		m_pRenderTargetViews[i] = nullptr;
}

D3D11RenderContext::~D3D11RenderContext()
{
	if (m_pRenderer->m_bReleaseResources && m_pDeviceContext)
		m_pDeviceContext->Release();
}

bool D3D11RenderContext::Setup()
{
	// Render Target
	m_pBackBuffer = new D3D11RenderTarget(m_pRenderer);
	m_pBackBuffer->InitializeBackbuffer();

	m_pRenderTargetViews[0] = m_pBackBuffer->GetRenderTargetView();

	// Blend State
	m_pBlendState = new D3D11BlendState();
	if (!m_pBlendState->Initialize(m_pRenderer->m_pDevice))
		return false;

	// Surface
	if (!m_surface.Setup())
		return false;

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

	// Done
	return true;
}

void D3D11RenderContext::Shutdown()
{
	delete m_pBlendState;
	m_pBlendState = nullptr;

	delete m_pBackBuffer;
	m_pBackBuffer = nullptr;
}

void D3D11RenderContext::SetViewportSize(const Vector2& size)
{
	m_size = size;

	m_viewport.Width = static_cast<float>(size.x);
	m_viewport.Height = static_cast<float>(size.y);
	m_viewport.MaxDepth = 1.0f;
}

void D3D11RenderContext::Render()
{
	if (m_pDeviceContext == nullptr)
		return;

	// Setup render state
	m_pBackBuffer->Apply(this);
	m_pDeviceContext->RSSetViewports(1, &m_viewport);

	if(m_pBlendState)
		m_pBlendState->Apply(this);
}
D3D_PRIMITIVE_TOPOLOGY PrimitiveTopologyToD3DEnum(PrimitiveTopology eTopology)
{
	switch (eTopology)
	{
	case PrimitiveTopology::PointList:
		return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

	case PrimitiveTopology::LineList:
		return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
	
	case PrimitiveTopology::LineStrip:
		return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;

	case PrimitiveTopology::TriangleList:
		return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	case PrimitiveTopology::TriangleStrip:
		return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

	default:
		return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	}
}

void D3D11RenderContext::Draw(PrimitiveTopology eTopology, size_t stNumElements)
{
	m_pDeviceContext->IASetPrimitiveTopology(PrimitiveTopologyToD3DEnum(eTopology));
	m_pDeviceContext->Draw(stNumElements, 0);
}
void D3D11RenderContext::DrawIndexed(PrimitiveTopology eTopology, size_t stNumElements)
{
	m_pDeviceContext->IASetPrimitiveTopology(PrimitiveTopologyToD3DEnum(eTopology));
	m_pDeviceContext->DrawIndexed(stNumElements, 0, 0);
}

void D3D11RenderContext::SaveState()
{
	m_stateSaver.saveCurrentState(m_pDeviceContext);
}
void D3D11RenderContext::RestoreState()
{
	m_stateSaver.restoreSavedState();
}

IRenderTarget* D3D11RenderContext::GetBackBufferRenderTarget() { return m_pBackBuffer; }
void D3D11RenderContext::SetRenderTargets(size_t stNumRenderTargets, IRenderTarget** ppRenderTargets, IRenderTarget* pDepthStencilView)
{
	for (size_t i = 0; i < stNumRenderTargets; i++)
		m_pRenderTargetViews[i] = reinterpret_cast<D3D11RenderTarget*>(ppRenderTargets[i])->GetRenderTargetView();

	//ID3D11DepthStencilView* pDepthStencilView_ = (pDepthStencilView == nullptr) ? nullptr : reinterpret_cast<D3D11RenderTarget*>(pDepthStencilView)->GetDepthStencilView();
	m_pDeviceContext->OMSetRenderTargets(stNumRenderTargets, m_pRenderTargetViews, nullptr);// pDepthStencilView_);
}