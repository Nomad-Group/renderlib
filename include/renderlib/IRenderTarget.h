#pragma once

enum class RenderTargetFormat : uint8_t
{
	RGBA_UInt32,
	RGBA_Float,
	DephStencil_Z328S, // 32bit Z-buffer, 8-bit stencil
};

struct IRenderTarget
{
    virtual ~IRenderTarget() = default;

    virtual bool Initialize(const math::Vector2& size, RenderTargetFormat eFormat = RenderTargetFormat::RGBA_UInt32) = 0;
	virtual bool InitializeDepthStencil(const math::Vector2& size, RenderTargetFormat eFormat) = 0;

    virtual bool IsBackbuffer() const = 0;

    virtual void Apply(IRenderContext*) = 0;
    virtual void Clear(IRenderContext*, const math::RGBA& col) = 0;
    virtual void ClearStencil(IRenderContext*) = 0;

	// Bind as Shader Resource
	virtual void Bind(IRenderContext*, ShaderType eShaderType, uint8_t uiSlot) = 0;

    virtual bool CopyTexture(IRenderContext*, IRenderTexture* pTexture) = 0;
};