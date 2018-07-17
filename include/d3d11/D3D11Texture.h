#pragma once
#include "renderlib/renderlib.h"
#include <DirectXMath.h>

using XMFLOAT4A = DirectX::XMFLOAT4A;
using XMFLOAT3  = DirectX::XMFLOAT3;
using XMFLOAT2  = DirectX::XMFLOAT2;

struct TextureVertex
{
	XMFLOAT3 pos;
	XMFLOAT2 tex0;
};

class D3D11Renderer;
class D3D11ShaderBundle;

class D3D11Texture : public IRenderTexture
{
private:
	D3D11Renderer* m_pRenderer;

	// Settings
    Vector2 m_position;
    Vector2 m_size;

	// Shader Resource View
	ID3D11ShaderResourceView* m_pResourceView = nullptr;
	bool InitializeShaderResourceView();

	// Vertex
	ID3D11Buffer* m_pVertexBuffer = nullptr;
	bool m_bUpdateVertexBuffer = false;
	TextureVertex m_vertices[6];
	bool UpdateVertexBuffer(IRenderContext*);

	// Texture
	ID3D11Texture2D* m_pTexture = nullptr;	

public:
	D3D11Texture(D3D11Renderer*);
	virtual ~D3D11Texture();

	static bool SetupContext(D3D11Renderer*, D3D11RenderContext*);
	void Release();

	// Loader
	bool LoadFrom2DTexture(IRenderContext*, ID3D11Texture2D* pTexture);
	virtual bool LoadFromMemory(IRenderContext*, uint8_t* pImage, uint32_t uiWidth, uint32_t uiHeight, ColorFormat format) override;
	virtual bool LoadFromPNG(IRenderContext*, const std::string& path) override;
	
	// Update
	virtual bool BlitFromMemory(IRenderContext*, uint8_t* pImage, uint32_t rowPitch, const Vector2& position, const Vector2& size) override;
	
	// Settings
    virtual const Vector2& GetSize() const override { return m_size; };
    virtual void SetPosition(const Vector2&) override;
    virtual const Vector2& GetPosition() const override { return m_position; };

	// Render
    virtual bool IsValid() const override { return m_pTexture != nullptr; };
	virtual void Render(IRenderContext*) override;
};
