#include "d3d11/D3D11Renderer.h"
#include "d3d11/D3D11RenderContext.h"
#include "d3d11/D3D11Rect.h"
#include "d3d11/D3D11ShaderBundle.h"
#include "d3d11/D3D11Shaders.h"

D3D11Rect::D3D11Rect(D3D11Renderer* pRenderer) :
    m_pRenderer(pRenderer)
{}

D3D11Rect::~D3D11Rect()
{
    delete m_pShaderBundle;
	delete m_pBuffer;
}

bool D3D11Rect::Initialize()
{
    m_pDevice = m_pRenderer->GetDevice();

    // Shader
    m_pShaderBundle = new D3D11ShaderBundle(m_pRenderer);
    m_pShaderBundle->SetShaders(d3d11_rect_shader_quad, d3d11_rect_shader_color);
    if (!m_pShaderBundle->Initialize())
        return false;

    // Buffer
	ZeroMemory(&m_shaderConstants, sizeof(m_shaderConstants));

	m_pBuffer = m_pRenderer->CreateBuffer(BufferType::Constant, sizeof(m_shaderConstants), BufferUsage::Dynamic);
	if (!m_pBuffer->Initialize(&m_shaderConstants))
		return false;

    // Done
    return true;
}

void D3D11Rect::DrawRect(D3D11RenderContext* pRenderContext, const Rectf& rect, const RGBA& color)
{
	auto pDeviceContext = pRenderContext->GetDeviceContext();

    // Setup
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    m_pShaderBundle->Apply(pRenderContext);
	m_pBuffer->Apply(pRenderContext);

    // Matrix
	const Vector2f& viewport = pRenderContext->GetViewportSize();

    m_shaderConstants.TransformMatrix[0] = rect.w * 2.0f / viewport.x;
    m_shaderConstants.TransformMatrix[12] = -1.0f + rect.x * 2.0f / viewport.x;
    m_shaderConstants.TransformMatrix[5] = rect.h * -2.0f / viewport.y;
    m_shaderConstants.TransformMatrix[13] = 1.0f + rect.y * -2.0f / viewport.y;
    m_shaderConstants.TransformMatrix[10] = 1.0f;
    m_shaderConstants.TransformMatrix[15] = 1.0f;

    // Color
    m_shaderConstants.Color[0] = static_cast<float>(color.r) / 255.0f;
    m_shaderConstants.Color[1] = static_cast<float>(color.g) / 255.0f;
    m_shaderConstants.Color[2] = static_cast<float>(color.b) / 255.0f;
    m_shaderConstants.Color[3] = static_cast<float>(color.a) / 255.0f;

	// Update Constant Buffer
	void* pBufferData = nullptr;
	if (!m_pBuffer->Map(pRenderContext, BufferAccess::WriteDiscard, &pBufferData))
		return;
	memcpy(pBufferData, &m_shaderConstants, sizeof(m_shaderConstants));
	m_pBuffer->Unmap(pRenderContext);

    // Draw
	pDeviceContext->Draw(4, 0);
}
