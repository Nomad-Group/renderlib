#pragma once
#include "D3D11Renderer.h"

#include <vector>
#include <mutex>

class D3D11RenderQueue : public IRenderQueue
{
	D3D11Renderer* m_pRenderer;
	std::vector<_detail::IRenderCommand*> m_vRenderCommands;
	std::recursive_mutex m_mutex;

public:
	struct FontInfo
	{
		IRenderFont* pFont = nullptr;

		float size = 14.0f;
		RGBA color;
	} m_fontInfo;

	Vector2 m_renderOffset;

public:
	D3D11RenderQueue(D3D11Renderer* p_renderer);
	virtual ~D3D11RenderQueue() = default;

	// Lock
	virtual void Lock() override;
	virtual void Unlock() override;

	// Queue
	virtual void Clear() override;
	virtual void AddCommand(_detail::IRenderCommand* pCommand) override;

	// Render
	virtual void Render(IRenderContext*) override;

	// Offset
	virtual void SetRenderOffset(const Vector2& o) override { m_renderOffset = o; };
	virtual void AddRenderOffset(const Vector2& o) override { m_renderOffset += o; };
	virtual const Vector2& GetRenderOffset() override { return m_renderOffset; };

	// Render Target
	virtual void SetRenderTarget(IRenderTarget*) override;
	virtual void ClearRenderTarget(IRenderTarget*, const RGBA&) override;

	// Draw
	virtual void DrawRect(const Rect& rect, const RGBA&) override;
	virtual void DrawTexture(const Vector2&, IRenderTexture* p_texture) override;

	// Text
	virtual void SetFont(const std::string& name, float size, const RGBA& color) override;
	virtual void SetFont(IRenderFont*) override;

	virtual void TextDrawString(const Vector2& position, const std::string& str) override;
	virtual void TextDrawStringEx(const Vector2& position, const Vector2& size, const std::string& str, FontRenderFlags::Enum eFlags) override;
};
