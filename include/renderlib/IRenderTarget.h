#pragma once

struct IRenderTarget
{
    virtual ~IRenderTarget() = default;

    virtual bool Initialize(const math::Vector2& size, TextureFormat eFormat = TextureFormat::RGBA_UInt32) = 0;

	virtual IRenderTexture* GetTexture() = 0;
    virtual bool IsBackbuffer() const = 0;

    virtual void Apply(IRenderContext*) = 0;
    virtual void Clear(IRenderContext*, const math::RGBA& col) = 0;
};