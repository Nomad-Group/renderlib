#pragma once
#include "renderlib/renderlib.h"
#include "FW1FontWrapper/FW1FontWrapper.h"
#include "D3D11Renderer.h"

class D3D11Font : public IRenderFont
{
	D3D11Renderer* m_pRenderer = nullptr;
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pDeviceContext = nullptr;

	IFW1FontWrapper* m_pFontWrapper = nullptr;

	// Settings
	const std::string m_sName;
	float m_fSize = 16.0f;
	RGBA m_color;

public:
	D3D11Font(D3D11Renderer* pRenderer, std::string name);
	~D3D11Font() override;

	//
	bool Initialize();

	//
	const std::string& GetName() const override { return m_sName; };
	float GetSize() const override { return m_fSize; };
	const RGBA& GetColor() const override { return m_color; };

	//
	void SetSize(const float fSize) override { m_fSize = fSize; }
	void SetColor(const RGBA& color) override { m_color = color; };

	//
	void DrawString(float x, float y, const std::string& str) override;
    void DrawStringW(float x, float y, const std::wstring& str) override;
	void DrawStringEx(float x, float y, float w, float h, const std::string& str, FontRenderFlags::Enum eFlags) override;
	void MeasureString(const std::string& str, float* pWidth, float* pHeight, FontRenderFlags::Enum eFlags) override;
    void MeasureStringW(const std::wstring& str, float* pWidth, float* pHeight, FontRenderFlags::Enum eFlags) override;
};
