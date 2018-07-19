#pragma once

enum class ColorFormat : uint8_t
{
    RGBA,
    BGRA
};

enum class TextureFormat : uint8_t
{
	RGBA_UInt32,
	RGBA_Float32,
	DepthStencil_Float32_UInt8, // 32bit Z-buffer, 8-bit Stencil
	Depth_Float32, // 32bit Z-buffer
};

enum class TextureBinding : uint8_t
{
	ShaderResource,
	RenderTarget,	// Shader Resource + Render Target
	DepthStencil,	// Shader Resource + Depth Stencil
};

struct IRenderTexture
{
	virtual ~IRenderTexture() = default;

    // Loader
    virtual bool LoadFromMemory(uint8_t* pImage, uint32_t uiWidth, uint32_t uiHeight, ColorFormat format = ColorFormat::RGBA) = 0;

	// LoadFromFile
	// uses stb_image internally
	// supported formats: jpg, png, tga, bmp, psd, gif, hdr, pic, pnm
    virtual bool LoadFromFile(const std::string& path) = 0;
    
	// Update
	virtual bool BlitFromMemory(IRenderContext*, uint8_t* pImage, uint32_t rowPitch, const math::Vector2& position, const math::Vector2& size) = 0;

	// Bind as Shader Resource
	virtual void Bind(IRenderContext*, ShaderType eShaderType, uint8_t uiSlot) = 0;

	// Getter
	virtual const math::Vector2& GetSize() const = 0;
    virtual bool IsValid() const = 0;
};
