#pragma once
#include <d3d11.h>

class D3D11BlendState
{
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pDeviceContext = nullptr;
	ID3D11BlendState* m_pBlendState = nullptr;

public:
	~D3D11BlendState();

	bool Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	void Apply();
};
