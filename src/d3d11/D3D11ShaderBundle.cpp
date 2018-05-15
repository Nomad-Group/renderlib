#include "d3d11/D3D11Renderer.h"
#include "d3d11/D3D11ShaderBundle.h"

D3D11ShaderBundle::D3D11ShaderBundle(D3D11Renderer* pRenderer) :
    m_pRenderer(pRenderer) {}

D3D11ShaderBundle::~D3D11ShaderBundle() {
    if (m_pPixelShader)
        m_pPixelShader->Release();
    if (m_pVertexShader)
        m_pVertexShader->Release();
}

void D3D11ShaderBundle::SetShaders(const char* vertex_shader, const char* pixel_shader) {
    m_sVertexShader = vertex_shader;
    m_sPixelShader = pixel_shader;
}

void D3D11ShaderBundle::SetupInputLayout(D3D11_INPUT_ELEMENT_DESC* pDesc, uint32_t uiSize) {
    m_pInputDescription = pDesc;
    m_uiInputDescSize = uiSize;
}

bool D3D11ShaderBundle::Initialize() {
    m_pDevice = m_pRenderer->GetDevice();
    m_pDeviceContext = m_pRenderer->GetDeviceContext();

    // Quad Shader
    ID3DBlob* pVertexShaderCode = nullptr;
    if (FAILED(CompileShader(m_sVertexShader,
        strlen(m_sVertexShader),
        nullptr,
        nullptr,
        nullptr,
        "VS",
        "vs_4_0",
        0,
        0,
        &pVertexShaderCode,
        nullptr))) {
        if (pVertexShaderCode)
            pVertexShaderCode->Release();

        return false;
    }

    // Create Vertex Shader
    if (FAILED(m_pDevice->CreateVertexShader(pVertexShaderCode->GetBufferPointer(),
        pVertexShaderCode->GetBufferSize(),
        nullptr,
        &m_pVertexShader))) {
        if (pVertexShaderCode)
            pVertexShaderCode->Release();

        return false;
    }

    // Input Layout
    if (m_uiInputDescSize > 0 && m_pInputDescription) {
        Apply();

        if (FAILED(m_pDevice->CreateInputLayout(m_pInputDescription,
            m_uiInputDescSize,
            pVertexShaderCode->GetBufferPointer(),
            pVertexShaderCode->GetBufferSize(),
            &m_pInputLayout))) {
            if (pVertexShaderCode)
                pVertexShaderCode->Release();

            return false;
        }
    }

    // Vertex Shader
    if (pVertexShaderCode)
        pVertexShaderCode->Release();

    // Compile Pixel Shader
    ID3DBlob* pPixelShaderCode = nullptr;
    if (FAILED(CompileShader(m_sPixelShader,
        strlen(m_sPixelShader),
        nullptr,
        nullptr,
        nullptr,
        "PS",
        "ps_4_0",
        0,
        0,
        &pPixelShaderCode,
        nullptr))) {
        if (pPixelShaderCode)
            pPixelShaderCode->Release();

        return false;
    }

    // Create Pixel Shader
    if (FAILED(m_pDevice->CreatePixelShader(pPixelShaderCode->GetBufferPointer(),
        pPixelShaderCode->GetBufferSize(),
        NULL,
        &m_pPixelShader))) {
        if (pPixelShaderCode)
            pPixelShaderCode->Release();

        return false;
    }

    // Pixel Shader
    if (pPixelShaderCode)
        pPixelShaderCode->Release();

    // Done
    return true;
}

void D3D11ShaderBundle::Apply(ID3D11Buffer* pBuffer) const {
    if (m_pInputLayout)
        m_pDeviceContext->IASetInputLayout(m_pInputLayout);

    m_pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);
    m_pDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
    m_pDeviceContext->GSSetShader(nullptr, nullptr, 0);

    if (pBuffer) {
        m_pDeviceContext->PSSetConstantBuffers(0, 1, &pBuffer);
        m_pDeviceContext->VSSetConstantBuffers(0, 1, &pBuffer);
    }
}

HRESULT D3D11ShaderBundle::CompileShader(const LPCVOID pSrcData, const SIZE_T SrcDataSize, const LPCSTR pFileName,
                                         CONST D3D_SHADER_MACRO* pDefines, ID3DInclude* pInclude, const LPCSTR pEntrypoint,
                                         const LPCSTR pTarget, const UINT Flags1, const UINT Flags2, ID3DBlob** ppCode,
                                         ID3DBlob** ppErrorMsgs) {
    // Find D3DCompile
    HRESULT(*pD3DCompile)(LPCVOID, SIZE_T, LPCSTR,
        CONST D3D_SHADER_MACRO*, ID3DInclude*, LPCSTR,
        LPCSTR, UINT, UINT, ID3DBlob**, ID3DBlob**) = nullptr;

    if (!pD3DCompile) {
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
            if (!name) break;

            const auto h_lib = LoadLibraryA(name);

            if (h_lib)
            {
                pD3DCompile = reinterpret_cast<decltype(pD3DCompile)>(GetProcAddress(h_lib, "D3DCompile"));

                if (pD3DCompile) break;
            }
        }

        if (!pD3DCompile) return E_FAIL;
    }

    // Compile
    return pD3DCompile(pSrcData, SrcDataSize, pFileName, pDefines, pInclude, pEntrypoint, pTarget, Flags1, Flags2,
                       ppCode, ppErrorMsgs);
}
