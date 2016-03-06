
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
	float3 g_color;
	float g_blend;
};
/*
struct ParamDraw
{
	Float3 color;
	float blend;
};
*/

float4 PS(VS_OUTPUT input) : SV_Target
{
	const float4 srcColor = texture0.Sample(sampler0, input.tex) * input.color;

	return float4(lerp(srcColor.rgb, g_color, g_blend), srcColor.a);
}
