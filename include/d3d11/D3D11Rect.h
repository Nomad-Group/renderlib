#pragma once
#include "D3D11Renderer.h"
using namespace math;

#include <d3d11.h>

class D3D11Rect
{
	D3D11Renderer* m_pRenderer = nullptr;
	ID3D11Device* m_pDevice = nullptr;

	D3D11ShaderBundle* m_pShaderBundle = nullptr;
	IVideoBuffer* m_pBuffer = nullptr;

	struct ShaderConstants {
		float TransformMatrix[16];
		float Color[4];
	} m_shaderConstants;

public:
	D3D11Rect(D3D11Renderer* pRenderer);
	~D3D11Rect();

	// Init
	bool Initialize();
	
	// Draw
	void DrawRect(D3D11RenderContext*, const Rectf& rect, const RGBA& color);
};
