#pragma once
#include <d3d11.h>

class D3D11BlendState
{
	ID3D11BlendState* m_pBlendState = nullptr;

public:
	~D3D11BlendState();

	bool Initialize(ID3D11Device* pDevice);
	void Apply(D3D11RenderContext*);
};
