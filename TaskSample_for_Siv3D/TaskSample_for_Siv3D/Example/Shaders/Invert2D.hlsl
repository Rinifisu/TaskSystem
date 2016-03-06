
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
	float4 srcColor = texture0.Sample(sampler0, input.tex);

	srcColor.rgb = float3(1.0, 1.0, 1.0) - srcColor.rgb;

	return srcColor * input.color;
}
