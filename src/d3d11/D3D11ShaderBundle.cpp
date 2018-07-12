#include "d3d11/D3D11Renderer.h"
#include "d3d11/D3D11RenderContext.h"
#include "d3d11/D3D11ShaderBundle.h"
#include "d3d11/D3D11Shader.h"

D3D11ShaderBundle::D3D11ShaderBundle(D3D11Renderer* pRenderer) :
    m_pRenderer(pRenderer)
{}

D3D11ShaderBundle::~D3D11ShaderBundle()
{
    if (m_pPixelShader)
        m_pPixelShader->Release();
    if (m_pVertexShader)
        m_pVertexShader->Release();
}

void D3D11ShaderBundle::SetShaders(const char* vertex_shader, const char* pixel_shader)
{
    m_sVertexShader = vertex_shader;
    m_sPixelShader = pixel_shader;
}

void D3D11ShaderBundle::SetupInputLayout(D3D11_INPUT_ELEMENT_DESC* pDesc, uint32_t uiSize)
{
    m_pInputDescription = pDesc;
    m_uiInputDescSize = uiSize;
}

bool D3D11ShaderBundle::Initialize()
{
    m_pDevice = m_pRenderer->GetDevice();

    // Quad Shader
    ID3DBlob* pVertexShaderCode = nullptr;
    if (FAILED(D3D11Shader::CompileShader(m_sVertexShader,
        strlen(m_sVertexShader),
        nullptr,
        nullptr,
        nullptr,
        "VS",
        "vs_4_0",
        0,
        0,
        &pVertexShaderCode,
        nullptr)))
	{
        if (pVertexShaderCode)
            pVertexShaderCode->Release();

        return false;
    }

    // Create Vertex Shader
    if (FAILED(m_pDevice->CreateVertexShader(pVertexShaderCode->GetBufferPointer(),
        pVertexShaderCode->GetBufferSize(),
        nullptr,
        &m_pVertexShader)))
	{
        if (pVertexShaderCode)
            pVertexShaderCode->Release();

        return false;
    }

	// Input Layout
	if (m_uiInputDescSize > 0 && m_pInputDescription)
	{
		// Apply?

		if (FAILED(m_pDevice->CreateInputLayout(m_pInputDescription,
			m_uiInputDescSize,
			pVertexShaderCode->GetBufferPointer(),
			pVertexShaderCode->GetBufferSize(),
			&m_pInputLayout)))
		{
			if (pVertexShaderCode)
				pVertexShaderCode->Release();

			return false;
		}
	}

    // Vertex Shader
    if (pVertexShaderCode)
        pVertexShaderCode->Release();

    // Compile Pixel Shader
    ID3DBlob* pPixelShaderCode = nullptr;
    if (FAILED(D3D11Shader::CompileShader(m_sPixelShader,
        strlen(m_sPixelShader),
        nullptr,
        nullptr,
        nullptr,
        "PS",
        "ps_4_0",
        0,
        0,
        &pPixelShaderCode,
        nullptr)))
	{
        if (pPixelShaderCode)
            pPixelShaderCode->Release();

        return false;
    }

    // Create Pixel Shader
    if (FAILED(m_pDevice->CreatePixelShader(pPixelShaderCode->GetBufferPointer(),
        pPixelShaderCode->GetBufferSize(),
        NULL,
        &m_pPixelShader)))
	{
        if (pPixelShaderCode)
            pPixelShaderCode->Release();

        return false;
    }

    // Pixel Shader
    if (pPixelShaderCode)
        pPixelShaderCode->Release();

    // Done
    return true;
}

void D3D11ShaderBundle::Apply(D3D11RenderContext* pRenderContext, ID3D11Buffer* pBuffer) const
{
	auto pDeviceContext = pRenderContext->GetDeviceContext();

	// Shader
    pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);
    pDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
    pDeviceContext->GSSetShader(nullptr, nullptr, 0);

	// Constant Buffer
    if (pBuffer)
	{
        pDeviceContext->PSSetConstantBuffers(0, 1, &pBuffer);
        pDeviceContext->VSSetConstantBuffers(0, 1, &pBuffer);
    }

	// Input Layout
	if (m_pInputLayout)
		pDeviceContext->IASetInputLayout(m_pInputLayout);
}
