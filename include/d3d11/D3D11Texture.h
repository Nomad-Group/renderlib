#pragma once
#include "renderlib/renderlib.h"
#include <DirectXMath.h>

using XMFLOAT4A = DirectX::XMFLOAT4A;
using XMFLOAT3  = DirectX::XMFLOAT3;
using XMFLOAT2  = DirectX::XMFLOAT2;

struct FRect
{
	float x;
	float y;
	float w;
	float h;
};

struct VertexPos
{
	XMFLOAT3 pos;
	XMFLOAT2 tex0;
};

class D3D11Renderer;
class D3D11ShaderBundle;

class D3D11Texture : public IRenderTexture
{
private:
	D3D11Renderer* m_pRenderer = nullptr;
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pDeviceContext;

	// Static Info
	static struct TextureDrawInfo
	{
		D3D11ShaderBundle* pShaderBundle = nullptr;
		ID3D11SamplerState* pSamplerState = nullptr;
	} m_textureDrawInfo;

	// Settings
    Vector2 m_position;
    Vector2 m_size;

	// Shader Resource View
	ID3D11ShaderResourceView* m_pResourceView = nullptr;
	bool InitializeShaderResourceView();

	// Vertex
	ID3D11Buffer* m_pVertexBuffer = nullptr;
	VertexPos m_vertices[6];
	bool UpdateVertexBuffer();

	// Texture
	ID3D11Texture2D* m_pTexture = nullptr;	

public:
	D3D11Texture(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~D3D11Texture();

	bool Initialize(D3D11Renderer* pRenderer);
	virtual void Release() override;

	// Load
	bool LoadFrom2DTexture(ID3D11Texture2D* pTexture);
    bool LoadFromMemory(uint8_t* pImage, uint32_t uiWidth, uint32_t uiHeight, ColorFormat format) override;
	bool LoadFromPNG(const std::string& path) override;
    bool BlitFromMemory(uint8_t* pImage, uint32_t rowPitch, const Vector2& position, const Vector2& size) override;
	
	// Settings
    virtual const Vector2& GetSize() const override { return m_size; };
    virtual void SetPosition(const Vector2&) override;
    virtual const Vector2& GetPosition() const override { return m_position; };

	// Render
    virtual bool IsValid() const override { return m_pTexture != nullptr; };
	void Render() override;
};
