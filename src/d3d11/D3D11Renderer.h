#pragma once
#include "renderlib/renderlib.h"
using namespace math;

#include <vector>
#include <map>
#include <d3d11.h>

#include "FW1FontWrapper/FW1FontWrapper.h"


struct IFW1Factory;
class D3D11Factory;
class D3D11Renderer;
class D3D11RenderContext;
class D3D11RenderTexture;
class D3D11BlendState;
class D3D11RenderTarget;
class D3D11RenderFont;
class D3D11RenderShader;
class D3D11ShaderBundle;
class D3D11ShaderInputLayout;
class D3D11VideoBuffer;

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
	void Present();

	// Resources
	virtual IRenderFont* GetFont(const std::string& name) override;
	virtual IRenderTexture* CreateTexture() override;
	virtual IRenderShader* CreateShader(const ShaderType) override;
	virtual IShaderInputLayout* CreateShaderInputLayout() override;
	virtual IVideoBuffer* CreateBuffer(const BufferType, const size_t, const BufferUsage) override;
	virtual IRenderTarget* CreateRenderTarget(const Vector2& size) override;
};
