#pragma once

struct IRenderer
{
	virtual ~IRenderer() = 0;
	virtual void Shutdown() = 0;

	// Viewport
	virtual const Vector2& GetViewportSize() const = 0;
    virtual void GetViewportSize(float& outWidth, float& outHeight) const = 0;
    virtual void SetViewportSize(const Vector2&) = 0;

	// Basic Drawing
	virtual void DrawRect(const Vector2& position, const Vector2& size, const RGBA& color) = 0;

	// Resources
	virtual IRenderFont* GetFont(const std::string& name) = 0;
	virtual IRenderTexture* GetTexture(const std::string& path) = 0;
    virtual IRenderTexture* CreateTexture() = 0;

	// Render Target
	virtual IRenderTarget* CreateRenderTarget(const Vector2& size) = 0;
	virtual IRenderTarget* GetBackBufferRenderTarget() = 0;
};
