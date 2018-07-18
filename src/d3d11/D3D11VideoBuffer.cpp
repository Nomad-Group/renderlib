#include "d3d11/D3D11VideoBuffer.h"
#include "d3d11/D3D11Renderer.h"
#include "d3d11/D3D11RenderContext.h"

D3D11VideoBuffer::D3D11VideoBuffer(D3D11Renderer* pRenderer, const BufferType eType, const size_t stSize, const BufferUsage eUsage) :
	m_pRenderer(pRenderer),
	m_eType(eType),
	m_stSize(stSize),
	m_eUsage(eUsage)
{}

D3D11VideoBuffer::~D3D11VideoBuffer()
{
	if (m_pBuffer)
		m_pBuffer->Release();
}

bool D3D11VideoBuffer::Initialize(void* pInitialData)
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));

	desc.ByteWidth = m_stSize;
	
	// Bind Flags "Type"
	switch (m_eType)
	{
	case BufferType::Vertex:
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		break;

	case BufferType::Index:
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		break;

	case BufferType::Constant:
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		break;

	default:
		return false;
	}
	
	// Usage
	switch (m_eUsage)
	{
	case BufferUsage::Default:
		desc.Usage = D3D11_USAGE_DEFAULT;
		break;

	case BufferUsage::Immutable:
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		break;

	case BufferUsage::Dynamic:
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		break;

	case BufferUsage::Staging:
		desc.Usage = D3D11_USAGE_STAGING;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
		break;

	default:
		return false;
	}

	// Create Buffer
	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(data));
	data.pSysMem = pInitialData;

	return SUCCEEDED(m_pRenderer->GetDevice()->CreateBuffer(&desc, &data, &m_pBuffer));
}

void D3D11VideoBuffer::Apply(IRenderContext* pRenderContext, size_t stIndexOrSize, size_t stOffset)
{
	auto pDeviceContext = reinterpret_cast<D3D11RenderContext*>(pRenderContext)->GetDeviceContext();
	switch (m_eType)
	{
	case BufferType::Vertex:
	{
		UINT stride = stIndexOrSize;
		UINT offset = stOffset;
		pDeviceContext->IASetVertexBuffers(0, 1, &m_pBuffer, &stride, &offset);
	} break;

	case BufferType::Index:
		pDeviceContext->IASetIndexBuffer(m_pBuffer, DXGI_FORMAT_R32_UINT, stOffset);
		break;

	case BufferType::Constant:
	{
		pDeviceContext->VSSetConstantBuffers(stIndexOrSize, 1, &m_pBuffer);
		pDeviceContext->PSSetConstantBuffers(stIndexOrSize, 1, &m_pBuffer);
	} break;

	default:
		return;
	}
}

bool D3D11VideoBuffer::IsAccessAllowed(BufferAccess eAccess) const
{
	switch (m_eUsage)
	{
	case BufferUsage::Dynamic:
		return eAccess == BufferAccess::Write || eAccess == BufferAccess::WriteDiscard;

	case BufferUsage::Staging:
		return true;

	//case BufferUsage::Default:
	//case BufferUsage::Immutable:
	default:
		return false;
	}
}

bool D3D11VideoBuffer::Map(IRenderContext* pRenderContext, BufferAccess eAccess, void** ppData)
{
	if (m_pBuffer == nullptr || !IsAccessAllowed(eAccess))
		return false;

	D3D11_MAP mapType;
	switch (eAccess)
	{
	case BufferAccess::Read:
		mapType = D3D11_MAP_READ;
		break;

	case BufferAccess::Write:
		mapType = D3D11_MAP_WRITE;
		break;

	case BufferAccess::ReadWrite:
		mapType = D3D11_MAP_READ_WRITE;
		break;

	case BufferAccess::WriteDiscard:
		mapType = D3D11_MAP_WRITE_DISCARD;
		break;

	default:
		return false;
	}

	auto pDeviceContext = reinterpret_cast<D3D11RenderContext*>(pRenderContext)->GetDeviceContext();

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(pDeviceContext->Map(m_pBuffer, 0, mapType, 0, &mappedResource)))
		return false;

	*ppData = mappedResource.pData;
	return true;
}

void D3D11VideoBuffer::Unmap(IRenderContext* pRenderContext)
{
	auto pDeviceContext = reinterpret_cast<D3D11RenderContext*>(pRenderContext)->GetDeviceContext();
	pDeviceContext->Unmap(m_pBuffer, 0);
}