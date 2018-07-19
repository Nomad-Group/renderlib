#include "D3D11ShaderSampler.h"
#include "D3D11RenderContext.h"

D3D11ShaderSampler::D3D11ShaderSampler(D3D11Renderer* pRenderer) :
	m_pRenderer(pRenderer)
{}

D3D11ShaderSampler::~D3D11ShaderSampler()
{
	if (m_pSamplerState)
		m_pSamplerState->Release();
}

bool D3D11ShaderSampler::Create()
{
	D3D11_SAMPLER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));

	desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	desc.MaxAnisotropy = 1;
	desc.MaxLOD = D3D11_FLOAT32_MAX;

	// my
	desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	// rastertek
	desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	// Create
	return SUCCEEDED(m_pRenderer->GetDevice()->CreateSamplerState(&desc, &m_pSamplerState));
}

void D3D11ShaderSampler::Bind(IRenderContext* pRenderContext, ShaderType eShaderType, uint8_t uiSlot)
{
	auto pDeviceContext = reinterpret_cast<D3D11RenderContext*>(pRenderContext)->GetDeviceContext();

	switch (eShaderType)
	{
	case ShaderType::Pixel:
		pDeviceContext->PSSetSamplers(uiSlot, 1, &m_pSamplerState);
		break;

	case ShaderType::Vertex:
		pDeviceContext->VSSetSamplers(uiSlot, 1, &m_pSamplerState);
		break;
	}
}