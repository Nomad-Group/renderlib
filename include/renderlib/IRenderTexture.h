#pragma once

enum class ColorFormat : uint8_t
{
    RGBA,
    BGRA
};

struct IRenderTexture
{
	virtual ~IRenderTexture() = default;

    // Loader
    virtual bool LoadFromMemory(IRenderContext*, uint8_t* pImage, uint32_t uiWidth, uint32_t uiHeight, ColorFormat format = ColorFormat::RGBA) = 0;
    virtual bool LoadFromPNG(IRenderContext*, const std::string& path) = 0;
    
	// Update
	virtual bool BlitFromMemory(IRenderContext*, uint8_t* pImage, uint32_t rowPitch, const math::Vector2& position, const math::Vector2& size) = 0;

	// Settings
	virtual const math::Vector2& GetSize() const = 0;
	
	virtual void SetPosition(const math::Vector2&) = 0;
    virtual const math::Vector2& GetPosition() const = 0;

	// Render
    virtual bool IsValid() const = 0;
	virtual void Render(IRenderContext*) = 0;
};
