#pragma once
#include "renderlib/renderlib.h"
#include <d3d11.h>
#include <vector>

struct D3D11ShaderInputElement
{
	D3D11ShaderInputElement(const ShaderInputElement, const size_t stOffset, const size_t stSize);
	~D3D11ShaderInputElement() = default;

	D3D11_INPUT_ELEMENT_DESC desc;
	const ShaderInputElement eType;
	const size_t size;
};

class D3D11Renderer;
class D3D11ShaderInputLayout : public IShaderInputLayout
{
	D3D11Renderer* m_pRenderer;

	ID3D11InputLayout* m_pInputLayout = nullptr;
	size_t m_stSize;

	// Input Element
	std::vector<D3D11ShaderInputElement> m_elements;

public:
	D3D11ShaderInputLayout(D3D11Renderer*);
	virtual ~D3D11ShaderInputLayout();

	// Input Layout
	virtual size_t GetSize() const override { return m_stSize; };
	virtual size_t GetNumElements() const override { return m_elements.size(); };

	// Elements
	D3D11_INPUT_ELEMENT_DESC* GetInputElements();

	virtual void AddFloat(ShaderInputElement, uint8_t uiNumFloats) override;
};