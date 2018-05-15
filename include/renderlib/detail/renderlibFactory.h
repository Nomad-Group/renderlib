#pragma once

namespace renderlib
{
	struct IFactory
	{
		virtual ~IFactory() = 0;

		virtual const bool IsInitialized() const = 0;

		virtual IRenderer* GetRenderer() = 0;
		virtual IRenderContext* GetRenderContext() = 0;
	};
}