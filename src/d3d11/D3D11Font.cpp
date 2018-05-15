#include "d3d11/D3D11Renderer.h"
#include "d3d11/D3D11Font.h"
#include "FW1FontWrapper/FW1FontWrapper.h"
#include <locale>
#include <codecvt>

static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

D3D11Font::D3D11Font(D3D11Renderer* pRenderer, std::string name) :
    m_pRenderer(pRenderer),
    m_sName(name)
{}

D3D11Font::~D3D11Font()
{
    if (m_pFontWrapper)
        m_pFontWrapper->Release();
}

bool D3D11Font::Initialize()
{
    m_pDevice = m_pRenderer->GetDevice();
    m_pDeviceContext = m_pRenderer->GetDeviceContext();

    if (FAILED(m_pRenderer->GetFW1Factory()->CreateFontWrapper(m_pDevice, converter.from_bytes(m_sName).c_str(), &
        m_pFontWrapper))) {
        return false;
    }

    // Done
    return true;
}

void D3D11Font::DrawString(const Vector2f& pos, const std::string& str) const
{
    if (!m_pFontWrapper)
        return;

    uint32_t color = ((m_color.a & 0xff) << 24) + ((m_color.b & 0xff) << 16) + ((m_color.g & 0xff) << 8) + (
                         m_color.r & 0xff);

    m_pFontWrapper->DrawString(m_pDeviceContext,
                               converter.from_bytes(str).c_str(),
                               m_fSize,
                               pos.x,
                               pos.y,
                               color,
                               FW1_NOWORDWRAP);
}

void D3D11Font::DrawStringW(const Vector2f& pos, const std::wstring& str) const
{
    if (!m_pFontWrapper)
        return;

    uint32_t color = ((m_color.a & 0xff) << 24) + ((m_color.b & 0xff) << 16) + ((m_color.g & 0xff) << 8) + (
        m_color.r & 0xff);

    m_pFontWrapper->DrawString(m_pDeviceContext,
        str.c_str(),
        m_fSize,
        pos.x,
        pos.y,
        color,
        FW1_NOWORDWRAP);
}

void D3D11Font::DrawStringEx(const Rectf& rect, const std::string& str, FontRenderFlags::Enum eFlags) const
{
    if (!m_pFontWrapper)
        return;

    uint32_t color = ((m_color.a & 0xff) << 24) + ((m_color.b & 0xff) << 16) + ((m_color.g & 0xff) << 8) + (
                         m_color.r & 0xff);

    // Text-Wrap
    UINT flags = 0;
    if (eFlags & FontRenderFlags::NO_WORD_WRAP)
        flags |= FW1_NOWORDWRAP;
    if (eFlags & FontRenderFlags::ALIGN_HCENTER)
        flags |= FW1_CENTER;
    if (eFlags & FontRenderFlags::ALIGN_VCENTER)
        flags |= FW1_VCENTER;
    if (eFlags & FontRenderFlags::CLIP_RECT)
        flags |= FW1_CLIPRECT;

    // Draw
    FW1_RECTF fwRect {
        rect.x,
        rect.y,
        rect.x + rect.w,
        rect.y + rect.h
    };

    m_pFontWrapper->DrawString(m_pDeviceContext,
                               converter.from_bytes(str).c_str(),
                               nullptr,
                               m_fSize,
                               &fwRect,
                               color,
                               &fwRect,
                               nullptr,
                               flags);
}

void D3D11Font::MeasureString(const std::string& str, Vector2f& size, FontRenderFlags::Enum eFlags) const
{
    if (!m_pFontWrapper)
        return;

    // Convert
    static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    uint32_t color = ((m_color.a & 0xff) << 24) + ((m_color.b & 0xff) << 16) + ((m_color.g & 0xff) << 8) + (
                         m_color.r & 0xff);

    // Text-Wrap
    UINT flags = 0;
    if (eFlags & FontRenderFlags::NO_WORD_WRAP)
        flags |= FW1_NOWORDWRAP;
    if (eFlags & FontRenderFlags::ALIGN_HCENTER)
        flags |= FW1_CENTER;
    if (eFlags & FontRenderFlags::ALIGN_VCENTER)
        flags |= FW1_VCENTER;

    // Measure
    FW1_RECTF fwRect { 0, 0, size.x, size.y };
    auto rect = m_pFontWrapper->MeasureString(converter.from_bytes(str).c_str(),
                                              converter.from_bytes(m_sName).c_str(),
                                              m_fSize,
                                              &fwRect,
                                              flags);

    // Out
    size.x = rect.Left + rect.Right;
    size.y = rect.Top + rect.Bottom;
}

void D3D11Font::MeasureStringW(const std::wstring& str, Vector2f& size, FontRenderFlags::Enum eFlags) const
{
    if (!m_pFontWrapper)
        return;

    // Convert
    uint32_t color = ((m_color.a & 0xff) << 24) + ((m_color.b & 0xff) << 16) + ((m_color.g & 0xff) << 8) + (
        m_color.r & 0xff);

    // Text-Wrap
    UINT flags = 0;
    if (eFlags & FontRenderFlags::NO_WORD_WRAP)
        flags |= FW1_NOWORDWRAP;
    if (eFlags & FontRenderFlags::ALIGN_HCENTER)
        flags |= FW1_CENTER;
    if (eFlags & FontRenderFlags::ALIGN_VCENTER)
        flags |= FW1_VCENTER;

    // Measure
    FW1_RECTF fwRect { 0, 0, size.x, size.y };
    auto rect = m_pFontWrapper->MeasureString(str.c_str(),
        converter.from_bytes(m_sName).c_str(),
        m_fSize,
        &fwRect,
        flags);

    // Out
    size.x = rect.Left + rect.Right;
    size.y = rect.Top + rect.Bottom;
}
