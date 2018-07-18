#include "D3D11Renderer.h"
#include "D3D11RenderContext.h"
#include "D3D11ShaderBundle.h"
#include "D3D11Shader.h"
#include "D3D11ShaderInputLayout.h"

D3D11ShaderBundle::D3D11ShaderBundle(D3D11Renderer* pRenderer) :
    m_pRenderer(pRenderer)
{}

D3D11ShaderBundle::~D3D11ShaderBundle()
{
	delete m_pVertexShader;
	delete m_pPixelShader;
}

void D3D11ShaderBundle::SetShaders(const char* vertex_shader, const char* pixel_shader)
{
    m_sVertexShader = vertex_shader;
    m_sPixelShader = pixel_shader;
}

bool D3D11ShaderBundle::Initialize()
{
    m_pDevice = m_pRenderer->GetDevice();

	// Vertex Shader
	m_pVertexShader = m_pRenderer->CreateShader(ShaderType::Vertex);
	if (!m_pVertexShader->Compile(m_sVertexShader, nullptr))
		return false;

	// Pixel Shader
	m_pPixelShader = m_pRenderer->CreateShader(ShaderType::Pixel);
	if (!m_pPixelShader->Compile(m_sPixelShader, nullptr))
		return false;

    // Done
    return true;
}

bool D3D11ShaderBundle::SetInputLayout(IShaderInputLayout* pInputLayout)
{
	if(m_pVertexShader)
		return m_pVertexShader->SetInputLayout(pInputLayout);

	return false;
}

void D3D11ShaderBundle::Apply(D3D11RenderContext* pRenderContext, ID3D11Buffer* pBuffer) const
{
	auto pDeviceContext = pRenderContext->GetDeviceContext();

	// Shader
	if (m_pPixelShader)
		m_pPixelShader->Apply(pRenderContext);
	if (m_pVertexShader)
		m_pVertexShader->Apply(pRenderContext);

	// Constant Buffer
    if (pBuffer)
	{
        pDeviceContext->PSSetConstantBuffers(0, 1, &pBuffer);
        pDeviceContext->VSSetConstantBuffers(0, 1, &pBuffer);
    }
}
