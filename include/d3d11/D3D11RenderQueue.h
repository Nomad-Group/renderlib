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
		std::string font = "Arial";
		float size = 14.0f;
		RGBA color;
	} m_fontInfo;

	Vector2 m_renderOffset;

public:
	D3D11RenderQueue(D3D11Renderer* p_renderer);
	virtual ~D3D11RenderQueue() = default;

	// Lock
    void Lock() override;
	void Unlock() override;

	// Queue
	void Clear() override;
	void AddCommand(_detail::IRenderCommand* pCommand) override;

	// Render
	void Render() override;

	// Offset
	virtual void SetRenderOffset(const Vector2& o) override { m_renderOffset = o; };
	virtual void AddRenderOffset(const Vector2& o) override { m_renderOffset += o; };
	virtual const Vector2& GetRenderOffset() override { return m_renderOffset; };

	// Render Target
	virtual void SetRenderTarget(IRenderTarget*) override;
	virtual void ClearRenderTarget(IRenderTarget*, const RGBA&) override;

	// Draw
	void DrawRect(const Vector2& position, const Vector2& size, const RGBA&) override;
	void DrawTexture(const Vector2&, IRenderTexture* p_texture) override;

	// Text
	void SetFont(const std::string& name, float size, const RGBA& color) override;
	void TextDrawString(const Vector2& position, const std::string& str) override;
	void TextDrawStringEx(const Vector2& position, const Vector2& size, const std::string& str, FontRenderFlags::Enum eFlags) override;
};
