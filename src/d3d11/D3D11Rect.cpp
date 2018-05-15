#include "d3d11/D3D11Renderer.h"
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
    m_pDeviceContext = m_pRenderer->GetDeviceContext();

    // Constants
    ZeroMemory(&shaderConstants, sizeof(shaderConstants));

    // Shader
    m_pShaderBundle = new D3D11ShaderBundle(m_pRenderer);
    m_pShaderBundle->SetShaders(d3d11_rect_shader_quad, d3d11_rect_shader_color);
    if (!m_pShaderBundle->Initialize()) {
        return false;
    }

    // Buffer
    D3D11_BUFFER_DESC bufferDesc;
    ZeroMemory(&bufferDesc, sizeof(bufferDesc));

    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = sizeof(ShaderConstants);
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    // Create Buffer
    if (m_pDevice->CreateBuffer(&bufferDesc, nullptr, &m_pBuffer) != S_OK) {
        if (m_pBuffer)
            m_pBuffer->Release();

        return false;
    }

    // Done
    return true;
}

void D3D11Rect::DrawRect(float x, float y, float w, float h, RGBA color)
{
    float viewportWidth, viewportHeight;
    m_pRenderer->GetViewportSize(viewportWidth, viewportHeight);

    // Setup
    m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    m_pShaderBundle->Apply(m_pBuffer);

    // Matrix
    shaderConstants.TransformMatrix[0] = w * 2.0f / viewportWidth;
    shaderConstants.TransformMatrix[12] = -1.0f + x * 2.0f / viewportWidth;
    shaderConstants.TransformMatrix[5] = h * -2.0f / viewportHeight;
    shaderConstants.TransformMatrix[13] = 1.0f + y * -2.0f / viewportHeight;
    shaderConstants.TransformMatrix[10] = 1.0f;
    shaderConstants.TransformMatrix[15] = 1.0f;

    // Color
    shaderConstants.Color[0] = static_cast<float>(color.r) / 255.0f;
    shaderConstants.Color[1] = static_cast<float>(color.g) / 255.0f;
    shaderConstants.Color[2] = static_cast<float>(color.b) / 255.0f;
    shaderConstants.Color[3] = static_cast<float>(color.a) / 255.0f;

    // Draw
    m_pDeviceContext->UpdateSubresource(m_pBuffer, 0, nullptr, &shaderConstants, 0, 0);
    m_pDeviceContext->Draw(4, 0);
}

void D3D11Renderer::DrawRect(const Vector2& position, const Vector2& size, const RGBA& color)
{
    if (m_pRect)
        m_pRect->DrawRect(
            static_cast<float>(position.x),
            static_cast<float>(position.y),
            static_cast<float>(size.x),
            static_cast<float>(size.y),
            color);
}
