#pragma once
#include <d3d11.h>
#include "D3D11Renderer.h"

class D3D11ShaderBundle
{
	D3D11Renderer* m_pRenderer = nullptr;
	ID3D11Device* m_pDevice = nullptr;
	
    const char* m_sVertexShader = nullptr;
    const char* m_sPixelShader = nullptr;

	D3D11_INPUT_ELEMENT_DESC* m_pInputDescription = nullptr;
	uint32_t m_uiInputDescSize = 0;

	ID3D11VertexShader* m_pVertexShader = nullptr;
	ID3D11PixelShader* m_pPixelShader = nullptr;
	ID3D11InputLayout* m_pInputLayout = nullptr;

public:
	D3D11ShaderBundle(D3D11Renderer* pRenderer);
	~D3D11ShaderBundle();

	//
	void SetShaders(const char* vertex_shader, const char* pixel_shader);
	void SetupInputLayout(D3D11_INPUT_ELEMENT_DESC* pDesc, uint32_t uiSize);

	bool Initialize();
	void Apply(D3D11RenderContext* pRenderContext, ID3D11Buffer* pConstantBuffer = nullptr) const;
};
