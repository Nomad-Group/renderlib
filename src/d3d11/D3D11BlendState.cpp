#include "d3d11/D3D11Renderer.h"
#include "d3d11/D3D11BlendState.h"

D3D11BlendState::~D3D11BlendState() {
    if (m_pBlendState)
        m_pBlendState->Release();
}

bool D3D11BlendState::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext) {
    m_pDevice = pDevice;
    m_pDeviceContext = pDeviceContext;

    D3D11_BLEND_DESC oBlendStateDesc;
	ZeroMemory(&oBlendStateDesc, sizeof(D3D11_BLEND_DESC));

	//oBlendStateDesc.AlphaToCoverageEnable = TRUE;

    //for (uint32_t a = 0; a < 8; ++a) {
        oBlendStateDesc.RenderTarget[0].BlendEnable = TRUE;
        oBlendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        oBlendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        oBlendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        oBlendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
        oBlendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
        oBlendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		oBlendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    //}

    // Blend State
    if (FAILED(m_pDevice->CreateBlendState(&oBlendStateDesc, &m_pBlendState))) {
        return false;
    }

    // Done
    return true;
}

void D3D11BlendState::Apply() {
    if (!m_pBlendState)
        return;

    const float blend_factor[4] = {0.f, 0.f, 0.f, 0.f};
	//const float blend_factor[4] = { 1.f, 1.f, 1.f, 1.f };
    m_pDeviceContext->OMSetBlendState(m_pBlendState, blend_factor, 0xffffffff);
}
