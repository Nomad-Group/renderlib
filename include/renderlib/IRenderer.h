#pragma once

// IRenderer is threadsafe
struct IRenderer
{
	virtual ~IRenderer() = 0;
	virtual void Shutdown() = 0;

	// Render Context
	virtual IRenderContext* GetRenderContext() = 0;

	// Resources
	virtual IRenderFont* GetFont(const std::string& name) = 0;
    virtual IRenderTexture* CreateTexture() = 0;

	// Render Target
	virtual IRenderTarget* CreateRenderTarget(const Vector2& size) = 0;
	virtual IRenderTarget* GetBackBufferRenderTarget() = 0;
};
