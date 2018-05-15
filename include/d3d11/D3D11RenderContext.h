#pragma once
#include "D3D11Renderer.h"

class D3D11RenderContext : public IRenderContext
{
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

public:
	D3D11RenderContext(D3D11Renderer*);
	virtual ~D3D11RenderContext() override;

	inline ID3D11DeviceContext* GetDeviceContext() { return m_pDeviceContext; };
	
	void Shutdown();
	bool Setup();

	void Render();

	// Viewport
	virtual const Vector2& GetViewportSize() const override { return m_size; };
	virtual void SetViewportSize(const Vector2&) override;

	// Basic Drawing
	virtual void DrawRect(const Rect& rect, const RGBA& color) override;
};