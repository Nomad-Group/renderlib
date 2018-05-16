#pragma once

namespace renderlib
{
	struct IFactory
	{
		virtual ~IFactory() = default;

		virtual const bool IsInitialized() const = 0;

		virtual IRenderer* GetRenderer() = 0;
		virtual IRenderContext* GetRenderContext() = 0;
		virtual IRenderQueue* CreateRenderQueue() = 0;

		virtual bool Render() = 0;
	};
}