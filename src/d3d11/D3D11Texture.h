#pragma once
#include "renderlib/renderlib.h"

class D3D11Renderer;
class D3D11ShaderBundle;

class D3D11Texture : public IRenderTexture
{
	friend class D3D11RenderContext;

private:
	D3D11Renderer* m_pRenderer;

	// Settings
    Vector2 m_position;
    Vector2 m_size;

	// Shader Resource View
	ID3D11ShaderResourceView* m_pShaderResourceView = nullptr;
	bool InitializeShaderResourceView();

	// Texture
	ID3D11Texture2D* m_pTexture = nullptr;	

public:
	D3D11Texture(D3D11Renderer*);
	virtual ~D3D11Texture();

	// Release
	void Release();

	// Loader
	bool LoadFrom2DTexture(IRenderContext*, ID3D11Texture2D* pTexture);
	virtual bool LoadFromMemory(IRenderContext*, uint8_t* pImage, uint32_t uiWidth, uint32_t uiHeight, ColorFormat format) override;
	virtual bool LoadFromPNG(IRenderContext*, const std::string& path) override;
	
	// Update
	virtual bool BlitFromMemory(IRenderContext*, uint8_t* pImage, uint32_t rowPitch, const Vector2& position, const Vector2& size) override;
	
	// Getter
    virtual const Vector2& GetSize() const override { return m_size; };
    virtual bool IsValid() const override { return m_pTexture != nullptr; };

	// Internal
	inline ID3D11ShaderResourceView* GetShaderResourceView() { return m_pShaderResourceView; };
};
