#pragma once
#include <vector>
#include <map>
#include <d3d11.h>
#include "renderlib/renderlib.h"

#include "FW1FontWrapper/FW1FontWrapper.h"
#include "FW1FontWrapper/CFW1StateSaver.h"

struct IFW1Factory;
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
	friend class D3D11RenderContext;

	// D3D11
	IDXGISwapChain* m_pSwapChain = nullptr;
	ID3D11Device* m_pDevice = nullptr;
	bool m_bReleaseResources = false; // if SwapChain was created by us

	// Render Context
	D3D11RenderContext* m_pRenderContext = nullptr;

	ID3D11RasterizerState* m_pRasterizerState = nullptr;
    ID3D11DepthStencilState* m_pDepthStencilState = nullptr;

    ID3D11VertexShader* m_pVertexShader = nullptr;
    ID3D11PixelShader* m_pPixelShader = nullptr;
    ID3D11InputLayout* m_pInputLayout = nullptr;
    ID3D11Buffer* m_pVertexConstantBuffer = nullptr;
    ID3D10Blob* m_pPixelShaderBlob = nullptr;
    ID3D10Blob* m_pVertexShaderBlob = nullptr;

	// Setup
	bool m_bIsSetUp = false;
	bool Setup();

	// Texture Draw Info
	struct TextureDrawInfo
	{
		D3D11ShaderBundle* pShaderBundle = nullptr;
		ID3D11SamplerState* m_pSamplerState = nullptr;
	} m_textureDrawInfo;

	// Fonts
	IFW1Factory* m_pFW1Factory = nullptr;
	std::vector<IRenderFont*> m_fonts;

    FW1FontWrapper::CFW1StateSaver m_stateSaver;

public:
	virtual ~D3D11Renderer() override;

	// Internal
	ID3D11Device* GetDevice() { return m_pDevice; };
	IFW1Factory* GetFW1Factory() { return m_pFW1Factory; };
    ID3D11DepthStencilState* GetDepthStencilState() { return m_pDepthStencilState; };

	//
	virtual void Shutdown() override;

	//
	void Reset();
	void Render(IDXGISwapChain*);
    void RestoreState();

	// Custom Swap Chain
	bool SetupNewSwapChain(HWND);
	void Present();

	// Resources
	virtual IRenderFont* GetFont(const std::string& name) override;
    virtual IRenderTexture* CreateTexture() override;

	virtual IRenderTarget* CreateRenderTarget(const Vector2& size) override;
	virtual IRenderTarget* GetBackBufferRenderTarget() override;
};
