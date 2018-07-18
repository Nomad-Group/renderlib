#pragma once
#include "D3D11Renderer.h"

class D3D11RenderSurface : public IRenderSurface
{
	D3D11Renderer* m_pRenderer;
	D3D11RenderContext* m_pRenderContext;

	// Texture
	struct TextureDrawInfo
	{
		TextureDrawInfo() = default;
		~TextureDrawInfo();

		D3D11ShaderBundle* pShaderBundle = nullptr;
		IShaderInputLayout* pInputLayout = nullptr;
		IVideoBuffer* pVertexBuffer = nullptr;
		ID3D11SamplerState* pSamplerState = nullptr;
	} m_textureDrawInfo;

	struct TextureVertex
	{
		float position[2];
		float texcoords[2];
	};

	// Rectangle
	struct RectangleDrawInfo
	{
		RectangleDrawInfo() = default;
		~RectangleDrawInfo();

		D3D11ShaderBundle* pShaderBundle = nullptr;
		IShaderInputLayout* pInputLayout = nullptr;
		IVideoBuffer* pVertexBuffer = nullptr;
		IVideoBuffer* pColorBuffer = nullptr;
	} m_rectangleDrawInfo;

	// Setup (Internal)
	bool SetupTexture();
	bool SetupRectangle();

public:
	D3D11RenderSurface(D3D11Renderer*, D3D11RenderContext*);
	~D3D11RenderSurface() = default;

	bool Setup();

	// Rectangle
	virtual void DrawRect(const Rect& rect, const RGBA& color) override;

	// Texture
	virtual void DrawTexture(IRenderTexture*, const math::Vector2&, const math::Vector2&) override;
};