#include "D3D11Renderer.h"
#include "D3D11RenderQueue.h"
#include "D3D11Texture.h"
#include "D3D11RenderTarget.h"

D3D11RenderQueue::D3D11RenderQueue(D3D11Renderer* pRenderer) :
    m_pRenderer(pRenderer)
{}

void D3D11RenderQueue::Clear()
{
    //std::lock_guard<std::recursive_mutex> lock(m_mutex);
	for (auto pCommand : m_vRenderCommands)
		delete pCommand;

	m_vRenderCommands.clear();
}

void D3D11RenderQueue::Lock()
{
    m_mutex.lock();
}

void D3D11RenderQueue::AddCommand(_detail::IRenderCommand* pCommand)
{
    //std::lock_guard<std::recursive_mutex> lock(m_mutex);
    m_vRenderCommands.push_back(pCommand);
}

void D3D11RenderQueue::Render(IRenderContext* pRenderContext)
{
    //std::lock_guard<std::recursive_mutex> lock(m_mutex);
    for (auto pCommand : m_vRenderCommands)
    {
		pCommand->Render(m_pRenderer, pRenderContext);
		delete pCommand;
    }

	m_vRenderCommands.clear();
}

void D3D11RenderQueue::Unlock()
{
    m_mutex.unlock();
}

// Render Target
struct D3D11SetRenderTargetCommand : _detail::IRenderCommand
{
	IRenderTarget* pRenderTarget;

	void Render(IRenderer* pRenderer, IRenderContext* pRenderContext) override
	{
		if (pRenderTarget)
			pRenderTarget->Apply(pRenderContext);
		else
			pRenderContext->GetBackBufferRenderTarget()->Apply(pRenderContext);
	}
};

void D3D11RenderQueue::SetRenderTarget(IRenderTarget* pRenderTarget)
{
	auto pCommand = new D3D11SetRenderTargetCommand();
	pCommand->pRenderTarget = pRenderTarget;

	AddCommand(pCommand);
}

struct D3D11ClearRenderTargetCommand : _detail::IRenderCommand
{
	IRenderTarget* pRenderTarget;
	RGBA color;

	void Render(IRenderer* pRenderer, IRenderContext* pRenderContext) override
	{
		if (pRenderTarget)
			pRenderTarget->Clear(pRenderContext, color);
		else
			pRenderContext->GetBackBufferRenderTarget()->Clear(pRenderContext, color);
	}
};

void D3D11RenderQueue::ClearRenderTarget(IRenderTarget* pRenderTarget, const RGBA& color)
{
	auto pCommand = new D3D11ClearRenderTargetCommand();
	pCommand->pRenderTarget = pRenderTarget;
	pCommand->color = color;

	AddCommand(pCommand);
}

// Font
void D3D11RenderQueue::SetFont(const std::string& name, float size, const RGBA& color)
{
    m_fontInfo = { m_pRenderer->GetFont(name), size, color };
}
void D3D11RenderQueue::SetFont(IRenderFont* pFont)
{
	if (pFont == nullptr)
		pFont = m_pRenderer->GetFont("Arial");

	m_fontInfo = { pFont, pFont->GetSize(), pFont->GetColor() };
}

// Rect
struct D3D11DrawRectCommand : _detail::IRenderCommand
{
	Rect rect;
    RGBA col;

    void Render(IRenderer* pRenderer, IRenderContext* pRenderContext) override
    {
        pRenderContext->GetSurface()->DrawRect(rect, col);
    }
};

void D3D11RenderQueue::DrawRect(const Rect& rect, const RGBA& color)
{
    auto pCommand = new D3D11DrawRectCommand();

	pCommand->rect = rect;
	pCommand->rect.x += m_renderOffset.x;
	pCommand->rect.y += m_renderOffset.y;

    pCommand->col = color;

    AddCommand(pCommand);
}

// Text
struct D3D11DrawTextCommand : _detail::IRenderCommand
{
    D3D11RenderQueue::FontInfo font;

	Vector2f position;
    std::string str;

    void Render(IRenderer* pRenderer, IRenderContext* pRenderContext) override
    {
        auto pFont = font.pFont == nullptr ? pRenderer->GetFont("Arial") : font.pFont;
        if (pFont)
		{
            pFont->SetSize(font.size);
            pFont->SetColor(font.color);
            pFont->DrawString(pRenderContext, position, str);
        }
    }
};

void D3D11RenderQueue::TextDrawString(const Vector2& position, const std::string& str)
{
    auto pCommand = new D3D11DrawTextCommand();
    pCommand->font = m_fontInfo;

	pCommand->position = m_renderOffset + position;
    pCommand->str = str;

    AddCommand(pCommand);
}

// Text
struct D3D11DrawTextExCommand : _detail::IRenderCommand
{
    D3D11RenderQueue::FontInfo font;
    FontRenderFlags::Enum fontFlags;

	Rectf rect;
    std::string str;

    void Render(IRenderer* pRenderer, IRenderContext* pRenderContext) override
    {
		auto pFont = font.pFont == nullptr ? pRenderer->GetFont("Arial") : font.pFont;
        if (pFont)
		{
            pFont->SetSize(font.size);
            pFont->SetColor(font.color);
            pFont->DrawStringEx(pRenderContext, rect, str, fontFlags);
        }
    }
};

void D3D11RenderQueue::TextDrawStringEx(const Vector2& position, const Vector2& size, const std::string& str, const FontRenderFlags::Enum eFlags)
{
    auto pCommand = new D3D11DrawTextExCommand();
    pCommand->font = m_fontInfo;
    pCommand->fontFlags = eFlags;

	pCommand->rect = Rectf(m_renderOffset + position, size);
    pCommand->str = str;

    AddCommand(pCommand);
}

// Textured Rect
struct D3D11DrawTextureCommand : _detail::IRenderCommand
{
    Vector2 position;
    IRenderTexture* pTexture;

    void Render(IRenderer* pRenderer, IRenderContext* pRenderContext) override
    {
		pRenderContext->GetSurface()->DrawTexture(pTexture, position);
    }
};

void D3D11RenderQueue::DrawTexture(const Vector2& pos, IRenderTexture* pTexture)
{
    const auto pCommand = new D3D11DrawTextureCommand();
    pCommand->position = m_renderOffset + pos;
    pCommand->pTexture = pTexture;

    AddCommand(pCommand);
}
