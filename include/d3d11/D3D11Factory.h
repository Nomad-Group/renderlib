#pragma once
#include "../renderlib/impl/renderlibD3D11.h"
#include "D3D11Renderer.h"
#include "D3D11RenderContext.h"

class D3D11Factory : public renderlib::ID3D11Factory
{
	IDXGISwapChain* m_pSwapChain = nullptr;
	bool m_bHookMode = false;
	bool m_bInitialized = false;

	D3D11Renderer m_renderer;
	D3D11RenderContext m_renderContext;

public:
	D3D11Factory();
	virtual ~D3D11Factory() override;

	virtual const bool IsInitialized() const override { return m_bInitialized; };
	virtual IRenderer* GetRenderer() override { return &m_renderer; };
	virtual IRenderContext* GetRenderContext() override { return &m_renderContext; };

	virtual bool Initialize(HWND hWindow) override;
	virtual bool InitializeFromHook(IDXGISwapChain*) override;

	virtual bool Present() override;
};