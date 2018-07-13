#pragma once

// IRenderContext is not threadsafe!
struct IRenderContext
{
	virtual ~IRenderContext() = default;

	// Viewport
	virtual const math::Vector2& GetViewportSize() const = 0;
	virtual void SetViewportSize(const math::Vector2&) = 0;

	virtual IRenderTarget* GetBackBufferRenderTarget() = 0;

	// Render
	virtual void Render() = 0;

	// Basic Drawing
	virtual void Draw(size_t stNumElements) = 0;
	virtual void DrawRect(const math::Rect& rect, const math::RGBA& color) = 0;

	// State Saver
	virtual void SaveState() = 0;
	virtual void RestoreState() = 0;
};