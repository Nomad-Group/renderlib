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
	virtual ~IRenderFont() = 0;

	// Settings
	virtual const std::string& GetName() const = 0;
	virtual float GetSize() const = 0;
	virtual const RGBA& GetColor() const = 0;
	
	// Settings
	virtual void SetSize(float) = 0;
	virtual void SetColor(const RGBA&) = 0;

	// Text
	virtual void DrawString(float x, float y, const std::string& str) = 0;
    virtual void DrawStringW(float x, float y, const std::wstring& str) = 0;
	virtual void DrawStringEx(float x, float y, float w, float h, const std::string& str, FontRenderFlags::Enum e_flags) = 0;
	virtual void MeasureString(const std::string& str, float* p_width, float* p_height, FontRenderFlags::Enum e_flags) = 0;
    virtual void MeasureStringW(const std::wstring& str, float* p_width, float* p_height, FontRenderFlags::Enum e_flags) = 0;
};
