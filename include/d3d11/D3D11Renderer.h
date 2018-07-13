#pragma once
#include <vector>
#include <map>
#include <d3d11.h>
#include "renderlib/renderlib.h"

#include "FW1FontWrapper/FW1FontWrapper.h"


struct IFW1Factory;
class D3D11Factory;
class D3D11Renderer;
class D3D11RenderContext;
class D3D11Texture;
class D3D11ShaderBundle;
class D3D11BlendState;
class D3D11RenderTarget;
class D3D11Rect;
class D3D11Font;

class D3D11Renderer : public IRenderer
{
	friend class D3D11Factory;
	friend class D3D11RenderContext;
	friend class D3D11RenderTarget;

	// D3D11
	IDXGISwapChain* m_pSwapChain = nullptr;
	ID3D11Device* m_pDevice = nullptr;
	bool m_bReleaseResources = false; // if SwapChain was created by us

	// Setup
	bool m_bIsSetUp = false;
	bool Setup();

	// Fonts
	IFW1Factory* m_pFW1Factory = nullptr;
	std::vector<IRenderFont*> m_fonts;

public:
	virtual ~D3D11Renderer() override;

	// Internal
	ID3D11Device* GetDevice() { return m_pDevice; };
	IFW1Factory* GetFW1Factory() { return m_pFW1Factory; };

	//
	virtual void Shutdown() override;

	void Reset();

	// Custom Swap Chain
	bool SetupNewSwapChain(HWND);
	void Present();

	// Resources
	virtual IRenderFont* GetFont(const std::string& name) override;
	virtual IRenderTexture* CreateTexture() override;
	virtual IRenderShader* CreateShader(const ShaderType) override;
	virtual IVideoBuffer* CreateBuffer(const BufferType, const size_t, const BufferUsage) override;
	virtual IRenderTarget* CreateRenderTarget(const Vector2& size) override;
};
