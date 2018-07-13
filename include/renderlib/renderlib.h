#pragma once
#include <string>
#include <stdint.h>

#include "detail/renderlibMath.h"
#include "detail/renderlibColors.h"

struct IRenderer;
struct IRenderContext;
struct IRenderFont;
struct IRenderTexture;
struct IRenderTarget;
struct IRenderShader;
struct IVideoBuffer;
struct IShaderInputLayout;

#include "IRenderShader.h" // (ShaderType enum)
#include "IVideoBuffer.h" // (BufferType, BufferUsage enum)

#include "IRenderer.h"
#include "IRenderContext.h"
#include "IRenderTexture.h"
#include "IRenderTarget.h"
#include "IRenderFont.h"
#include "IShaderInputLayout.h"

#include "IRenderQueue.h"

namespace renderlib { struct IFactory; };