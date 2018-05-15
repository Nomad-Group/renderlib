#pragma once

struct IRenderTarget
{
    virtual ~IRenderTarget() = 0;

    virtual bool Initialize(IRenderer* pRenderer, const Vector2& size) = 0;

    virtual bool IsBackbuffer() const = 0;

    virtual void Apply() = 0;
    virtual void Clear(const RGBA& col) = 0;
    virtual void ClearStencil() = 0;

    virtual bool CopyTexture(IRenderTexture* pTexture) = 0;
};