#pragma once

struct IShaderInputLayout
{
	virtual ~IShaderInputLayout() = default;

	// Input Layout
	virtual size_t GetSize() const = 0;
	virtual size_t GetNumElements() const = 0;

	// Elements
	virtual void AddFloat(const std::string&, uint8_t uiNumFloats) = 0;
};