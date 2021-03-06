#pragma once
#include "D3D11Renderer.h"
#include "D3D11RenderSurface.h"

#include "FW1FontWrapper/CFW1StateSaver.h"

class D3D11RenderContext : public IRenderContext
{
	friend class D3D11Factory;
	friend class D3D11Renderer;
	friend class D3D11RenderTexture;

	// Rendering
	D3D11Renderer* m_pRenderer;
	ID3D11DeviceContext* m_pDeviceContext = nullptr;

	D3D11BlendState* m_pBlendState = nullptr;
	D3D11RenderTarget* m_pBackBuffer = nullptr;

	D3D11RenderSurface m_surface;

	// State
	static constexpr const size_t MAX_RENDER_TARGET_VIEWS = 4;
	ID3D11RenderTargetView* m_pRenderTargetViews[MAX_RENDER_TARGET_VIEWS];

	// Viewport
	D3D11_VIEWPORT m_viewport;
	Vector2 m_size;

	// State Saver
	FW1FontWrapper::CFW1StateSaver m_stateSaver;

public:
	D3D11RenderContext(D3D11Renderer*);
	virtual ~D3D11RenderContext() override;
	
	// Render Context
	void Shutdown();
	bool Setup();

	// Render
	virtual void Render() override;

	// Viewport
	virtual const Vector2& GetViewportSize() const override { return m_size; };
	virtual void SetViewportSize(const Vector2&) override;

	// Render Targets
	virtual IRenderTarget* GetBackBufferRenderTarget() override;
	virtual void SetRenderTargets(size_t stNumRenderTargets, IRenderTarget**, IRenderTarget* pDepthStencilView) override;

	// Drawing
	virtual void Draw(PrimitiveTopology, size_t) override;
	virtual void DrawIndexed(PrimitiveTopology, size_t stNumElements) override;

	virtual IRenderSurface* GetSurface() override { return &m_surface; };

	// State Saver
	virtual void SaveState() override;
	virtual void RestoreState() override;

	// Internal
	inline ID3D11DeviceContext* GetDeviceContext() { return m_pDeviceContext; };
};