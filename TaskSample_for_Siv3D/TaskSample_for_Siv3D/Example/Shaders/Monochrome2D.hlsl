
Texture2D texture0 : register( t0 );
SamplerState sampler0 : register( s0 );

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float4 color : COLOR0;
};

float4 PS(VS_OUTPUT input) : SV_Target
{
	const float4 srcColor = texture0.Sample(sampler0, input.tex);

	const float luminance = dot(srcColor.rgb, float3(0.299, 0.587, 0.114));

	return float4(luminance, luminance, luminance, srcColor.a) * input.color;
}
