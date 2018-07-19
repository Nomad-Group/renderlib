#pragma once

enum class PrimitiveTopology
{
	PointList,
	LineList,
	LineStrip,
	TriangleList,
	TriangleStrip
};

// IRenderContext is not threadsafe!
struct IRenderContext
{
	virtual ~IRenderContext() = default;

	// Viewport
	virtual const math::Vector2& GetViewportSize() const = 0;
	virtual void SetViewportSize(const math::Vector2&) = 0;

	// Render Target
	virtual IRenderTarget* GetBackBufferRenderTarget() = 0;
	virtual void SetRenderTargets(size_t stNumRenderTargets, IRenderTarget**, IRenderTarget* pDepthStencilView = nullptr) = 0;

	// Render
	virtual void Render() = 0;

	// Drawing
	virtual void Draw(PrimitiveTopology, size_t stNumElements) = 0;
	virtual void DrawIndexed(PrimitiveTopology, size_t stNumElements) = 0;

	virtual IRenderSurface* GetSurface() = 0;

	// State Saver
	virtual void SaveState() = 0;
	virtual void RestoreState() = 0;
};