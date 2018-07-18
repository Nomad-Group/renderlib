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
	"	float2 texCoords : TEXCOORD;"
	"};"
	"struct PS_Input"
	"{"
	"	float4 pos : SV_POSITION;"
	"	float2 texCoords : TEXCOORD;"
	"};"
	"PS_Input VS(VS_Input vertex)"
	"{"
	"	PS_Input vsOut = (PS_Input)0;"
	"	vsOut.pos = float4(vertex.pos.x, vertex.pos.y, 0.0f, 1.0f);"
	"	vsOut.texCoords = vertex.texCoords;"
	"	return vsOut;"
	"}"
	"float4 PS(PS_Input frag) : SV_TARGET"
	"{"
	"	return colorMap.Sample(colorSampler, frag.texCoords);"
	"}";

// Line    
static char d3d11_line_shaders[] =
    "struct VSOut"
    "{"
    "	float4 Col : COLOR;"
    "	float4 Pos : SV_POSITION;"
    "};"

    "VSOut VS(float4 Col : COLOR, float4 Pos : POSITION)"
    "{"
    "	VSOut Output;"
    "	Output.Pos = Pos;"
    "	Output.Col = Col;"
    "	return Output;"
    "}"

    "float4 PS(float4 Col : COLOR) : SV_TARGET"
    "{"
    "	return Col;"
    "}";

// Texture Input Layout
static D3D11_INPUT_ELEMENT_DESC d3d11_texture_input[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};
