#pragma once
#include "D3D11Renderer.h"

class D3D11ShaderSampler :
	public IShaderSampler
{
	D3D11Renderer* m_pRenderer;

	// Shader Sampler
	ID3D11SamplerState* m_pSamplerState = nullptr;

public:
	D3D11ShaderSampler(D3D11Renderer*);
	~D3D11ShaderSampler();

	// Create
	virtual bool Create() override;

	// Bind as Shader Resource
	virtual void Bind(IRenderContext*, ShaderType eShaderType, uint8_t uiSlot) override;
};