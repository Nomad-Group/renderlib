#pragma once
#include "renderlib/renderlib.h"
#include <d3d11.h>

class D3D11Renderer;
class D3D11VideoBuffer : public IVideoBuffer
{
	D3D11Renderer* m_pRenderer;
	const BufferType m_eType;
	const BufferUsage m_eUsage;
	const size_t m_stSize;

	ID3D11Buffer* m_pBuffer = nullptr;

public:
	D3D11VideoBuffer(D3D11Renderer*, const BufferType, const size_t, const BufferUsage);
	~D3D11VideoBuffer();

	// Initialize
	virtual bool Initialize(const void*) override;

	// Buffer
	virtual void Apply(IRenderContext*, size_t stIndexOrSize, size_t stOffset) override;

	virtual size_t GetSize() const override { return m_stSize; };
	virtual BufferUsage GetUsage() const override { return m_eUsage; };

	// Memory Access
	virtual bool IsAccessAllowed(BufferAccess) const override;

	virtual bool Map(IRenderContext*, BufferAccess, void**) override;
	virtual void Unmap(IRenderContext*) override;

	// Internal
	inline ID3D11Buffer* GetBuffer() { return m_pBuffer; };
};