#pragma once
#include "renderlib/renderlib.h"
#include <d3d11.h>

class D3D11Renderer;
class D3D11Shader : public IRenderShader
{
	D3D11Renderer* m_pRenderer;
	const ShaderType m_shaderType;

	// Shader: both ID3D11Vertex/PixelShader inherit ID3D11DeviceChild
	ID3D11DeviceChild* m_pShader = nullptr;

public:
	D3D11Shader(D3D11Renderer*, const ShaderType);
	virtual ~D3D11Shader();

	// Compiler
	virtual bool Compile(const char* strShader, const char* strEntrypoint) override;
	static HRESULT CompileShader(LPCVOID pSrcData, SIZE_T SrcDataSize, LPCSTR pFileName, CONST D3D_SHADER_MACRO* pDefines, ID3DInclude* pInclude, LPCSTR pEntrypoint, LPCSTR pTarget, UINT Flags1, UINT Flags2, ID3DBlob** ppCode, ID3DBlob** ppErrorMsgs);

	// Apply
	virtual void Apply(IRenderContext*) override;
};