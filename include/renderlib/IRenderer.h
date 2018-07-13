#pragma once

// IRenderer is threadsafe
struct IRenderer
{
	virtual ~IRenderer() = default;
	virtual void Shutdown() = 0;

	// Resources
	virtual IRenderFont* GetFont(const std::string& name) = 0;
    virtual IRenderTexture* CreateTexture() = 0;
	virtual IRenderShader* CreateShader(const ShaderType) = 0;
	virtual IShaderInputLayout* CreateShaderInputLayout() = 0;
	virtual IVideoBuffer* CreateBuffer(const BufferType, const size_t, const BufferUsage = BufferUsage::Default) = 0;
	virtual IRenderTarget* CreateRenderTarget(const math::Vector2& size) = 0;
};
