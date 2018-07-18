#pragma once
#include <d3d11.h>
#include <D3DCompiler.h>

// Rect
const char d3d11_rect_shader_quad[] =
	"cbuffer ShaderConstants : register(b0) {"
	"   float4x4 TransformMatrix : packoffset(c0);"
	"};"
	"float4 VS(uint VertexIndex : SV_VertexID) : SV_Position{"
	"   const float2 corners[4] = {"
	"      float2(0.0f, 1.0f),"
	"      float2(0.0f, 0.0f),"
	"      float2(1.0f, 1.0f),"
	"      float2(1.0f, 0.0f)"
	"   };"
	"   return mul(TransformMatrix, float4(corners[VertexIndex].xy, 0.0f, 1.0f));"
	"}";

const char d3d11_rect_shader_color[] =
	"cbuffer ShaderConstants : register(b0) {"
	"   float4 Color : packoffset(c4);"
	"};"
	"float4 PS() : SV_Target {"
	"   return Color;"
	"}";

// Texture
const char d3d11_texture_shader[] =
	"Texture2D colorMap_ : register(t0);"
	"SamplerState colorSampler_ : register(s0);"
	"struct VS_Input"
	"{"
	"	float4 pos : POSITION;"
	"	float2 tex0 : TEXCOORD0;"
	"};"
	"struct PS_Input"
	"{"
	"	float4 pos : SV_POSITION;"
	"	float2 tex0 : TEXCOORD0;"
	"};"
	"PS_Input VS(VS_Input vertex)"
	"{"
	"	PS_Input vsOut = (PS_Input)0;"
	"	vsOut.pos = vertex.pos;"
	"	vsOut.tex0 = vertex.tex0;"
	"	return vsOut;"
	"}"
	"float4 PS(PS_Input frag) : SV_TARGET"
	"{"
	"	return colorMap_.Sample(colorSampler_, frag.tex0);"
	"}";

const char d3d11_texture_shader_v2[] =
	"Texture2D colorMap : register(t0);"
	"SamplerState colorSampler : register(s0);"
	"struct VS_Input"
	"{"
	"	float2 pos : POSITION;"
	"	float2 texcoords : TEXCOORD0;"
	"};"
	"struct PS_Input"
	"{"
	"	float2 pos : SV_POSITION;"
	"	float2 texcoords : TEXCOORD0;"
	"};"
	"PS_Input VS(VS_Input vertex)"
	"{"
	"	PS_Input vsOut = (PS_Input)0;"
	"	vsOut.pos = vertex.pos;"
	"	vsOut.texcoords = vertex.texcoords;"
	"	return vsOut;"
	"}"
	"float4 PS(PS_Input frag) : SV_TARGET"
	"{"
	"	return colorMap.Sample(colorSampler, frag.texcoords);"
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
