#pragma once

struct FontRenderFlags
{
	enum Enum : uint8_t
	{
		NONE = 0,
		NO_WORD_WRAP = 1 << 1,
		ALIGN_HCENTER = 1 << 2,
		ALIGN_VCENTER = 1 << 3,
		CLIP_RECT = 1 << 4
	};
};

struct IRenderFont
{
	virtual ~IRenderFont() = default;

	// Properties
	virtual const std::string& GetName() const = 0;

	virtual void SetSize(float) = 0;
	virtual float GetSize() const = 0;

	virtual void SetColor(const RGBA&) = 0;
	virtual const RGBA& GetColor() const = 0;

	// Text
	virtual void DrawString(IRenderContext*, const Vector2f&, const std::string& str) const = 0;
    virtual void DrawStringW(IRenderContext*, const Vector2f&, const std::wstring& str) const = 0;
	virtual void DrawStringEx(IRenderContext*, const Rectf&, const std::string& str, FontRenderFlags::Enum eFlags) const = 0;
	
	virtual void MeasureString(const std::string& str, Vector2f& size, FontRenderFlags::Enum eFlags) const = 0;
    virtual void MeasureStringW(const std::wstring& str, Vector2f& size, FontRenderFlags::Enum eFlags) const = 0;
};
