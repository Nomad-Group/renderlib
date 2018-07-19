#pragma once
#include "D3D11Renderer.h"
#include "D3D11RenderTexture.h"

class D3D11RenderTarget : public IRenderTarget
{
	D3D11Renderer* m_pRenderer = nullptr;
	D3D11RenderTexture* m_pTexture = nullptr;
    ID3D11RenderTargetView* m_pRenderTargetView = nullptr;

	bool m_bBackbuffer = false;

public:
	D3D11RenderTarget(D3D11Renderer*);
	virtual ~D3D11RenderTarget() override;

    virtual bool Initialize(const Vector2&, TextureFormat) override;
	bool InitializeBackbuffer();
	
	virtual IRenderTexture* GetTexture() override { return m_pTexture; };
    virtual bool IsBackbuffer() const override { return m_bBackbuffer; };

	virtual void Apply(IRenderContext*) override;
	virtual void Clear(IRenderContext*, const RGBA& col) override;

	// Internal
	inline ID3D11RenderTargetView* GetRenderTargetView() { return m_pRenderTargetView; };
};
