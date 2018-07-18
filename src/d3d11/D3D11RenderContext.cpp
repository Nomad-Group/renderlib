#include "d3d11/D3D11RenderContext.h"
#include "d3d11/D3D11BlendState.h"
#include "d3d11/D3D11RenderTarget.h"
#include "d3d11/D3D11VideoBuffer.h"
#include "d3d11/D3D11ShaderBundle.h"
#include "d3d11/D3D11Shaders.h"

D3D11RenderContext::D3D11RenderContext(D3D11Renderer* pRenderer) :
	m_pRenderer(pRenderer),
	m_surface(pRenderer, this)
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

IRenderTarget* D3D11RenderContext::GetBackBufferRenderTarget() { return m_pRenderTarget; }