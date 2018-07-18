#pragma once

// IRenderSurface is not threadsafe!
struct IRenderSurface
{
	// Rectangle
	virtual void DrawRect(const math::Rect& rect, const math::RGBA& color) = 0;

	// Texture
	virtual void DrawTexture(IRenderTexture*, const math::Vector2& position, const math::Vector2& size = math::Vector2()) = 0;
};