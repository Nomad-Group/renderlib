#pragma once

enum class ShaderType : uint8_t
{
	Vertex,		// default entrypoint: VS
	Pixel,		// default entrypoint: PS
};

struct IRenderShader
{
	virtual ~IRenderShader() = default;

	// Shader
	virtual ShaderType GetType() const = 0;

	// Compiler
	virtual bool Compile(const char* strShader, const char* strEntrypoint = nullptr) = 0;

	// Input Layout
	virtual bool SetInputLayout(IShaderInputLayout*) = 0;
	
	// Apply
	virtual void Apply(IRenderContext*) = 0;
};