#pragma once
#include <vector>
#include <map>
#include <d3d11.h>
#include "renderlib/renderlib.h"

#include "FW1FontWrapper/FW1FontWrapper.h"
#include "FW1FontWrapper/CFW1StateSaver.h"

struct IFW1Factory;
class D3D11Renderer;
class D3D11Texture;
class D3D11ShaderBundle;
class D3D11BlendState;
class D3D11RenderTarget;
class D3D11Rect;
class D3D11Font;

class D3D11Renderer : public IRenderer
{
	// Game
	IDXGISwapChain* m_pSwapChain = nullptr;
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pDeviceContext = nullptr;
	bool m_bReleaseResources = false; // if SwapChain was created by us

	// D3D11 Stuff
	D3D11BlendState* m_pBlendState = nullptr;
	D3D11RenderTarget* m_pRenderTarget = nullptr;
	D3D11Rect* m_pRect = nullptr;
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

	// Viewport
	D3D11_VIEWPORT m_viewport;
    Vector2 m_size;

	// Texture Draw Info
	struct TextureDrawInfo
	{
		D3D11ShaderBundle* pShaderBundle = nullptr;
		ID3D11SamplerState* m_pSamplerState = nullptr;
	} m_textureDrawInfo;

	// Fonts
	IFW1Factory* m_pFW1Factory = nullptr;
	std::vector<IRenderFont*> m_vFonts;

	// Textures
	std::map<std::string, IRenderTexture*> m_mTextures;

    FW1FontWrapper::CFW1StateSaver m_stateSaver;
    void ApplyStates();

public:
	virtual ~D3D11Renderer() override;

	// Internal
	ID3D11Device* GetDevice() { return m_pDevice; };
	ID3D11DeviceContext* GetDeviceContext() { return m_pDeviceContext; };
	IFW1Factory* GetFW1Factory() { return m_pFW1Factory; };
    ID3D11DepthStencilState* GetDepthStencilState() { return m_pDepthStencilState; };
    D3D11BlendState* GetBlendState() { return m_pBlendState; };

	//
	virtual void Shutdown() override;

	//
	void Reset();
	void Render(IDXGISwapChain*);
    void RestoreState();

	// Custom Swap Chain
	bool SetupNewSwapChain(HWND);
	void Present();

	// Viewport
    virtual const Vector2& GetViewportSize() const override { return m_size; };
    virtual void GetViewportSize(float& outWidth, float& outHeight) const;
    virtual void SetViewportSize(const Vector2&) override;

	//
	virtual void DrawRect(const Vector2& position, const Vector2& size, const RGBA& color) override;
	
	// Resources
	virtual IRenderFont* GetFont(const std::string& name) override;
	virtual IRenderTexture* GetTexture(const std::string& path) override;
    virtual IRenderTexture* CreateTexture() override;

	virtual IRenderTarget* CreateRenderTarget(const Vector2& size) override;
	virtual IRenderTarget* GetBackBufferRenderTarget() override;

    inline Vector2 &GetSize() { return m_size; }
};
