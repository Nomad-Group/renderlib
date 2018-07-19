#include "D3D11ShaderInputLayout.h"
#include "D3D11Renderer.h"

D3D11ShaderInputLayout::D3D11ShaderInputLayout(D3D11Renderer* pRenderer) :
	m_pRenderer(pRenderer)
{}

D3D11ShaderInputElement::D3D11ShaderInputElement(const std::string _name, const size_t stOffset, const size_t stSize) :
	name(_name),
	size(stSize)
{
	ZeroMemory(&desc, sizeof(desc));

	desc.SemanticName = name.c_str();
	desc.AlignedByteOffset = stOffset;
	desc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
}

D3D11ShaderInputLayout::~D3D11ShaderInputLayout()
{
	if (m_pInputLayout)
		m_pInputLayout->Release();
}

void D3D11ShaderInputLayout::AddFloat(const std::string& name, uint8_t uiNumFloats)
{
	m_elements.emplace_back(name, m_stSize, sizeof(float) * uiNumFloats);
	D3D11ShaderInputElement& element = m_elements.back();
	m_stSize += element.size;

	// Format
	switch (uiNumFloats)
	{
	case 1: element.desc.Format = DXGI_FORMAT_R32_FLOAT; break;
	case 2:	element.desc.Format = DXGI_FORMAT_R32G32_FLOAT; break;
	case 3: element.desc.Format = DXGI_FORMAT_R32G32B32_FLOAT; break;
	case 4: element.desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; break;
	
	default:
		return;
	}
}

D3D11_INPUT_ELEMENT_DESC* D3D11ShaderInputLayout::GetInputElements()
{
	auto pInputElements = (D3D11_INPUT_ELEMENT_DESC*)malloc(sizeof(D3D11_INPUT_ELEMENT_DESC) * m_elements.size());
	size_t i = 0;
	for (const auto& element : m_elements)
	{
		pInputElements[i] = element.desc;
		pInputElements[i].SemanticName = element.name.c_str();
		i++;
	}

	return pInputElements;
}