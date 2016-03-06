
Texture2D texture0 : register( t0 );
SamplerState sampler0 : register( s0 );

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float4 color : COLOR0;
};

cbuffer psConstants1 : register( b1 )
{
	float2 g_textureSize;
	float2 g_offsetR;
	float2 g_offsetG;
	float2 g_offsetB;
	float2 g_offsetA;
};
/*
struct ParamRGBSplit
{
	Float2 textureSize;
	Float2 offsetR;
	Float2 offsetG;
	Float2 offsetB;
	Float2 offsetA;
	Float2 _unused;
};
*/

float4 PS(VS_OUTPUT input) : SV_Target
{
	const float2 uv = input.tex;
	const float2 texelSize = 1.0 / g_textureSize;

	const float r = texture0.Sample(sampler0, uv + texelSize * g_offsetR).r;
	const float g = texture0.Sample(sampler0, uv + texelSize * g_offsetG).g;
	const float b = texture0.Sample(sampler0, uv + texelSize * g_offsetB).b;
	const float a = texture0.Sample(sampler0, uv + texelSize * g_offsetA).a;

	return float4(r, g, b, a);
}
