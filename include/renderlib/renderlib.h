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

#include "IRenderShader.h" // (ShaderType enum)
#include "IRenderer.h"
#include "IRenderContext.h"
#include "IRenderTexture.h"
#include "IRenderTarget.h"
#include "IRenderFont.h"

#include "IRenderQueue.h"

namespace renderlib { struct IFactory; };