#pragma once
#include "D3D11Renderer.h"

#include "FW1FontWrapper/CFW1StateSaver.h"

class D3D11RenderContext : public IRenderContext
{
	friend class D3D11Factory;
	friend class D3D11Renderer;
	friend class D3D11Texture;

	D3D11Renderer* m_pRenderer;
	ID3D11DeviceContext* m_pDeviceContext = nullptr;

	D3D11BlendState* m_pBlendState = nullptr;
	D3D11RenderTarget* m_pRenderTarget = nullptr;
	D3D11Rect* m_pRect = nullptr;

	// Viewport
	D3D11_VIEWPORT m_viewport;
	Vector2 m_size;

	// Texture
	struct TextureDrawInfo
	{
		TextureDrawInfo() = default;
		~TextureDrawInfo() = default;

		D3D11ShaderBundle* pShaderBundle = nullptr;
		ID3D11SamplerState* pSamplerState = nullptr;
	} m_textureDrawInfo;

	// State Saver
	FW1FontWrapper::CFW1StateSaver m_stateSaver;

public:
	D3D11RenderContext(D3D11Renderer*);
	virtual ~D3D11RenderContext() override;

	inline ID3D11DeviceContext* GetDeviceContext() { return m_pDeviceContext; };
	
	void Shutdown();
	bool Setup();

	virtual void Render() override;

	// Viewport
	virtual const Vector2& GetViewportSize() const override { return m_size; };
	virtual void SetViewportSize(const Vector2&) override;

	virtual IRenderTarget* GetBackBufferRenderTarget() override;

	// Basic Drawing
	virtual void Draw(size_t) override;
	virtual void DrawIndexed(size_t stNumElements) override;

	virtual void DrawRect(const Rect& rect, const RGBA& color) override;

	// State Saver
	virtual void SaveState() override;
	virtual void RestoreState() override;
};