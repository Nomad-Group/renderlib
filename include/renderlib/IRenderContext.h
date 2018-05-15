#pragma once

// IRenderContext is not threadsafe!
struct IRenderContext
{
	virtual ~IRenderContext() = default;

	// Viewport
	virtual const Vector2& GetViewportSize() const = 0;
	virtual void SetViewportSize(const Vector2&) = 0;

	virtual IRenderTarget* GetBackBufferRenderTarget() = 0;

	// Basic Drawing
	virtual void DrawRect(const Rect& rect, const RGBA& color) = 0;

	// State Saver
	virtual void SaveState() = 0;
	virtual void RestoreState() = 0;
};