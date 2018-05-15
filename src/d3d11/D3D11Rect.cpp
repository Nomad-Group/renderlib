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

    if (m_pBuffer)
        m_pBuffer->Release();
}

bool D3D11Rect::Initialize()
{
    m_pDevice = m_pRenderer->GetDevice();

    // Constants
    ZeroMemory(&shaderConstants, sizeof(shaderConstants));

    // Shader
    m_pShaderBundle = new D3D11ShaderBundle(m_pRenderer);
    m_pShaderBundle->SetShaders(d3d11_rect_shader_quad, d3d11_rect_shader_color);
    if (!m_pShaderBundle->Initialize())
        return false;

    // Buffer
    D3D11_BUFFER_DESC bufferDesc;
    ZeroMemory(&bufferDesc, sizeof(bufferDesc));

    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = sizeof(ShaderConstants);
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    // Create Buffer
    if (m_pDevice->CreateBuffer(&bufferDesc, nullptr, &m_pBuffer) != S_OK)
	{
        if (m_pBuffer)
            m_pBuffer->Release();

        return false;
    }

    // Done
    return true;
}

void D3D11Rect::DrawRect(D3D11RenderContext* pRenderContext, const Rectf& rect, const RGBA& color)
{
	auto pDeviceContext = pRenderContext->GetDeviceContext();

    // Setup
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    m_pShaderBundle->Apply(pRenderContext, m_pBuffer);

    // Matrix
	const Vector2f& viewport = pRenderContext->GetViewportSize();

    shaderConstants.TransformMatrix[0] = rect.w * 2.0f / viewport.x;
    shaderConstants.TransformMatrix[12] = -1.0f + rect.x * 2.0f / viewport.x;
    shaderConstants.TransformMatrix[5] = rect.h * -2.0f / viewport.y;
    shaderConstants.TransformMatrix[13] = 1.0f + rect.y * -2.0f / viewport.y;
    shaderConstants.TransformMatrix[10] = 1.0f;
    shaderConstants.TransformMatrix[15] = 1.0f;

    // Color
    shaderConstants.Color[0] = static_cast<float>(color.r) / 255.0f;
    shaderConstants.Color[1] = static_cast<float>(color.g) / 255.0f;
    shaderConstants.Color[2] = static_cast<float>(color.b) / 255.0f;
    shaderConstants.Color[3] = static_cast<float>(color.a) / 255.0f;

    // Draw
	pDeviceContext->UpdateSubresource(m_pBuffer, 0, nullptr, &shaderConstants, 0, 0);
	pDeviceContext->Draw(4, 0);
}
