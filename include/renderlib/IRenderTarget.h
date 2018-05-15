#pragma once

struct IRenderTarget
{
    virtual ~IRenderTarget() = default;

    virtual bool Initialize(IRenderer* pRenderer, const Vector2& size) = 0;

    virtual bool IsBackbuffer() const = 0;

    virtual void Apply(IRenderContext*) = 0;
    virtual void Clear(IRenderContext*, const RGBA& col) = 0;
    virtual void ClearStencil(IRenderContext*) = 0;

    virtual bool CopyTexture(IRenderContext*, IRenderTexture* pTexture) = 0;
};