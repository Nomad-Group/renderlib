#include "d3d11/D3D11Shader.h"
#include "d3d11/D3D11Renderer.h"
#include "d3d11/D3D11RenderContext.h"

D3D11Shader::D3D11Shader(D3D11Renderer* pRenderer, const ShaderType shaderType) :
	m_pRenderer(pRenderer),
	m_shaderType(shaderType)
{}

D3D11Shader::~D3D11Shader()
{
	if(m_pShader)
		m_pShader->Release();
}

bool D3D11Shader::Compile(const char* strShader, const char* strEntrypoint)
{
	const char* strTarget = nullptr;
	switch (m_shaderType)
	{
	case ShaderType::Vertex:
	{
		strTarget = "vs_4_0";
		strEntrypoint = strEntrypoint == nullptr ? "VS" : strEntrypoint;
	} break;

	case ShaderType::Pixel:
	{
		strTarget = "ps_4_0";
		strEntrypoint = strEntrypoint == nullptr ? "PS" : strEntrypoint;
	} break;

	default:
		return false;
	}

	// Compile (Shader Code)
	ID3DBlob* pShaderCode = nullptr;
	if (FAILED(CompileShader(strShader,
		strlen(strShader),
		nullptr,
		nullptr,
		nullptr,
		strEntrypoint,
		strTarget,
		0,
		0,
		&pShaderCode,
		nullptr)))
	{
		if (pShaderCode)
			pShaderCode->Release();

		return false;
	}

	// Create Shader
	auto pDevice = m_pRenderer->GetDevice();
	bool success = false;

	switch (m_shaderType)
	{
	case ShaderType::Vertex:
		success = SUCCEEDED(pDevice->CreateVertexShader(pShaderCode->GetBufferPointer(),
			pShaderCode->GetBufferSize(),
			nullptr,
			(ID3D11VertexShader**)&m_pShader));
		break;

	case ShaderType::Pixel:
		success = SUCCEEDED(pDevice->CreatePixelShader(pShaderCode->GetBufferPointer(),
			pShaderCode->GetBufferSize(),
			nullptr,
			(ID3D11PixelShader**)&m_pShader));
		break;

	default:
		pShaderCode->Release();
		return false;
	}

	pShaderCode->Release();
	return success;
}

void D3D11Shader::Apply(IRenderContext* pRenderContext)
{
	auto pDeviceContext = reinterpret_cast<D3D11RenderContext*>(pRenderContext)->GetDeviceContext();

	switch (m_shaderType)
	{
	case ShaderType::Vertex:
		pDeviceContext->VSSetShader((ID3D11VertexShader*)m_pShader, nullptr, 0);
		break;

	case ShaderType::Pixel:
		pDeviceContext->PSSetShader((ID3D11PixelShader*)m_pShader, nullptr, 0);
		break;

	default:
		return;
	}
}

HRESULT D3D11Shader::CompileShader(const LPCVOID pSrcData, const SIZE_T SrcDataSize, const LPCSTR pFileName,
	CONST D3D_SHADER_MACRO* pDefines, ID3DInclude* pInclude, const LPCSTR pEntrypoint,
	const LPCSTR pTarget, const UINT Flags1, const UINT Flags2, ID3DBlob** ppCode,
	ID3DBlob** ppErrorMsgs)
{
	// Find D3DCompile
	HRESULT(WINAPI *pD3DCompile)(LPCVOID, SIZE_T, LPCSTR,
		CONST D3D_SHADER_MACRO*, ID3DInclude*, LPCSTR,
		LPCSTR, UINT, UINT, ID3DBlob**, ID3DBlob**) = nullptr;

	if (!pD3DCompile)
	{
		const char* compiler[] = {
			"d3dcompiler_47.dll",
			"d3dcompiler_46.dll",
			"d3dcompiler_45.dll",
			"d3dcompiler_44.dll",
			"d3dcompiler_43.dll",
			nullptr
		};

		for (const auto &name : compiler)
		{
			if (!name)
				break;

			const auto hLibrary = LoadLibraryA(name);
			if (hLibrary)
			{
				pD3DCompile = reinterpret_cast<decltype(pD3DCompile)>(GetProcAddress(hLibrary, "D3DCompile"));
				if (pD3DCompile)
					break;
			}
		}

		if (!pD3DCompile)
			return E_FAIL;
	}

	// Compile
	return pD3DCompile(pSrcData, SrcDataSize, pFileName, pDefines, pInclude, pEntrypoint, pTarget, Flags1, Flags2,
		ppCode, ppErrorMsgs);
}