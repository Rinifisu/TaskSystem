
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
	float g_strength;
	float g_angle;
};
/*
struct ParamSwirl
{
	float strength;
	float angle;
	Float2 _unused;
};
*/

float4 PS(VS_OUTPUT input) : SV_Target
{
	float2 uv = input.tex - 0.5;

	const float len = length(uv);

	const float rad = len * g_strength + g_angle;

	const float c = cos(rad), s = sin(rad);

	uv = mul(uv, float2x2(c, -s, s, c)) + 0.5;

	const float4 srcColor = texture0.Sample(sampler0, uv);

	return srcColor * input.color;
}
