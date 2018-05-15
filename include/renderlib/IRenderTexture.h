#pragma once

enum class ColorFormat : uint8_t
{
    RGBA,
    BGRA
};

struct IRenderTexture
{
	virtual ~IRenderTexture() = 0;
	virtual void Release() = 0;

    // Loader
    virtual bool LoadFromMemory(IRenderContext*, uint8_t* pImage, uint32_t uiWidth, uint32_t uiHeight, ColorFormat format = ColorFormat::RGBA) = 0;
    virtual bool LoadFromPNG(IRenderContext*, const std::string& path) = 0;
    virtual bool BlitFromMemory(IRenderContext*, uint8_t* pImage, uint32_t rowPitch, const Vector2& position, const Vector2& size) = 0;

	// Settings
	virtual const Vector2& GetSize() const = 0;
	
	virtual void SetPosition(const Vector2&) = 0;
    virtual const Vector2& GetPosition() const = 0;

	// Render
    virtual bool IsValid() const = 0;
	virtual void Render(IRenderContext*) = 0;
};
