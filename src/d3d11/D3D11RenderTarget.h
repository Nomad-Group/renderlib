#pragma once
#include "D3D11Renderer.h"
#include "D3D11RenderTexture.h"

class D3D11RenderTarget : public IRenderTarget
{
	D3D11Renderer* m_pRenderer = nullptr;
	ID3D11Texture2D* m_pRenderTexture = nullptr;
    ID3D11RenderTargetView* m_pRenderTargetView = nullptr;

	ID3D11ShaderResourceView* m_pShaderResourceView = nullptr;
	ID3D11DepthStencilView* m_pDepthStencilView = nullptr;

	bool m_bBackbuffer = false;

public:
	D3D11RenderTarget(D3D11Renderer*);
	virtual ~D3D11RenderTarget() override;

    virtual bool Initialize(const Vector2&, RenderTargetFormat) override;
	virtual bool InitializeDepthStencil(const math::Vector2&, RenderTargetFormat) override;
	bool InitializeBackbuffer();
	
    virtual bool IsBackbuffer() const override { return m_bBackbuffer; };

	virtual void Apply(IRenderContext*) override;
	virtual void Clear(IRenderContext*, const RGBA& col) override;
	virtual void ClearStencil(IRenderContext*) override;

	// Bind
	virtual void Bind(IRenderContext*, ShaderType eShaderType, uint8_t uiSlot) override;

	virtual bool CopyTexture(IRenderContext*, IRenderTexture* pTexture) override;

	// Internal
	inline ID3D11RenderTargetView* GetRenderTargetView() { return m_pRenderTargetView; };
	inline ID3D11DepthStencilView* GetDepthStencilView() { return m_pDepthStencilView; };
};
