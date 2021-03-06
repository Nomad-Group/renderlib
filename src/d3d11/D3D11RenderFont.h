#pragma once
#include "renderlib/renderlib.h"
using namespace math;

#include "FW1FontWrapper/FW1FontWrapper.h"
#include "D3D11Renderer.h"

class D3D11RenderFont : public IRenderFont
{
	D3D11Renderer* m_pRenderer = nullptr;
	ID3D11Device* m_pDevice = nullptr;

	IFW1FontWrapper* m_pFontWrapper = nullptr;

	// Settings
	const std::string m_sName;
	float m_fSize = 16.0f;
	RGBA m_color;

public:
	D3D11RenderFont(D3D11Renderer* pRenderer, std::string name);
	virtual ~D3D11RenderFont() override;

	bool Initialize();

	// Properties
	virtual const std::string& GetName() const override { return m_sName; };

	virtual void SetSize(const float fSize) override { m_fSize = fSize; };
	virtual float GetSize() const override { return m_fSize; };

	virtual void SetColor(const RGBA& color) override { m_color = color; };
	virtual const RGBA& GetColor() const override { return m_color; };

	// Text
	virtual void DrawString(IRenderContext*, const Vector2f&, const std::string&) const override;
	virtual void DrawStringW(IRenderContext*, const Vector2f&, const std::wstring&) const override;
	virtual void DrawStringEx(IRenderContext*, const Rectf&, const std::string&, FontRenderFlags::Enum) const override;
	
	virtual void MeasureString(const std::string&, Vector2f&, FontRenderFlags::Enum) const override;
	virtual void MeasureStringW(const std::wstring&, Vector2f&, FontRenderFlags::Enum) const override;
};
