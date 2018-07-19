#pragma once

enum class ShaderInputElement
{
	Position,
	Texcoord,
	Color
};

struct IShaderInputLayout
{
	virtual ~IShaderInputLayout() = default;

	// Input Layout
	virtual size_t GetSize() const = 0;
	virtual size_t GetNumElements() const = 0;

	// Elements
	virtual void AddFloat(ShaderInputElement, uint8_t uiNumFloats) = 0;
};