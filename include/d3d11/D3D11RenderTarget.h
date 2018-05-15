#pragma once
#include "D3D11Renderer.h"
#include "D3D11Texture.h"

class D3D11RenderTarget : public IRenderTarget
{
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pDeviceContext = nullptr;

	ID3D11RenderTargetView* m_pPreviousRT = nullptr;
    ID3D11DepthStencilView* m_pPreviousDSV = nullptr;

	ID3D11Texture2D* m_pRenderTexture = nullptr;
    ID3D11RenderTargetView* m_pRenderTargetView = nullptr;

	ID3D11ShaderResourceView* m_pShaderResourceView = nullptr;
	ID3D11DepthStencilView* m_pDepthStencilView = nullptr;

	bool m_bBackbuffer = false;

public:
	D3D11RenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~D3D11RenderTarget();

    virtual bool Initialize(IRenderer* pRenderer, const Vector2&) override;
	bool InitializeBackbuffer(IDXGISwapChain* pSwapChain);
	
    virtual bool IsBackbuffer() const override { return m_bBackbuffer; };

	virtual void Apply() override;
	virtual void Clear(const RGBA& col = { 0, 0, 0, 0 }) override;
	virtual void ClearStencil() override;

	virtual bool CopyTexture(IRenderContext*, IRenderTexture* pTexture) override;
};
