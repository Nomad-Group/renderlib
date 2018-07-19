#pragma once

struct IShaderSampler
{
	virtual ~IShaderSampler() = default;

	// Create
	virtual bool Create() = 0;

	// Bind as Shader Resource
	virtual void Bind(IRenderContext*, ShaderType eShaderType, uint8_t uiSlot) = 0;
};