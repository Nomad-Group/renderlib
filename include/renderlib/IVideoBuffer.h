#pragma once

enum class BufferType : uint8_t
{
	Vertex,
	Index,
	Constant
};

enum class BufferUsage : uint8_t
{
	Default,		// CPU --, GPU rw
	Immutable,		// CPU --, GPU r-
	Dynamic,		// CPU -w, GPU r-
	Staging,		// CPU rw, GPU rw
};

enum class BufferAccess : uint8_t
{
	Read,			// read  only
	Write,			// write only
	ReadWrite,		// read/write
	WriteDiscard	// write only, current discarded
};

struct IVideoBuffer
{
	virtual ~IVideoBuffer() = default;

	// Initialize
	virtual bool Initialize(void* pInitialData = nullptr) = 0;

	// Buffer
	// for constant buffer: stIndexOrSize = slot
	virtual void Apply(IRenderContext*, size_t stIndexOrSize = 0, size_t stOffset = 0) = 0;

	virtual size_t GetSize() const = 0;
	virtual BufferUsage GetUsage() const = 0;

	// Memory Access
	virtual bool IsAccessAllowed(BufferAccess) const = 0;

	virtual bool Map(IRenderContext*, BufferAccess, void** ppData) = 0;
	virtual void Unmap(IRenderContext*) = 0;
};