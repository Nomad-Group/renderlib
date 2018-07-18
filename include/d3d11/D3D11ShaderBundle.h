#pragma once
#include <d3d11.h>
#include "D3D11Renderer.h"

class D3D11ShaderBundle
{
	D3D11Renderer* m_pRenderer = nullptr;
	ID3D11Device* m_pDevice = nullptr;
	
    const char* m_sVertexShader = nullptr;
    const char* m_sPixelShader = nullptr;

	IRenderShader* m_pVertexShader = nullptr;
	IRenderShader* m_pPixelShader = nullptr;
	D3D11ShaderInputLayout* m_pInputLayout = nullptr;

public:
	D3D11ShaderBundle(D3D11Renderer* pRenderer);
	~D3D11ShaderBundle();

	void SetShaders(const char* vertex_shader, const char* pixel_shader);
	bool Initialize();
	bool SetInputLayout(D3D11ShaderInputLayout* pInputLayout);
	
	void Apply(D3D11RenderContext* pRenderContext, ID3D11Buffer* pConstantBuffer = nullptr) const;
};
