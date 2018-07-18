#pragma once
#include <d3d11.h>
#include <D3DCompiler.h>

// Rect
const char d3d11_rect_shader[] =
	"cbuffer ShaderConstant"
	"{"
	"	float4 color : COLOR;"
	"};"
	"float4 VS(float2 pos : POSITION) : SV_POSITION"
	"{"
	"	return float4(pos.x, pos.y, 0.0f, 1.0f);"
	"}"
	"float4 PS() : SV_TARGET"
	"{"
	"	return color;"
	"}";

// Texture
const char d3d11_texture_shader[] =
	"Texture2D colorMap : register(t0);"
	"SamplerState colorSampler : register(s0);"
	"struct VS_Input"
	"{"
	"	float2 pos : POSITION;"
	"	float2 texCoord : TEXCOORD;"
	"};"
	"struct PS_Input"
	"{"
	"	float4 pos : SV_POSITION;"
	"	float2 texCoord : TEXCOORD;"
	"};"
	"PS_Input VS(VS_Input input)"
	"{"
	"	PS_Input output;"
	"	output.pos = float4(input.pos.x, input.pos.y, 0.0f, 1.0f);"
	"	output.texCoord = input.texCoord;"
	"	return output;"
	"}"
	"float4 PS(PS_Input input) : SV_TARGET"
	"{"
	"	return colorMap.Sample(colorSampler, input.texCoord);"
	"}";

// Line    
static char d3d11_line_shaders[] =
    "struct PS_Input"
    "{"
    "	float4 col : COLOR;"
    "	float4 pos : SV_POSITION;"
    "};"

    "PS_Input VS(float4 pos : POSITION, float4 col : COLOR)"
    "{"
    "	PS_Input output;"
    "	output.pos = pos;"
    "	output.col = col;"
    "	return output;"
    "}"

    "float4 PS(PS_Input input) : SV_TARGET"
    "{"
    "	return input.col;"
    "}";