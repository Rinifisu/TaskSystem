
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
	float g_radius;
};
/*
struct ParamPoissonDisk
{
	Float2 textureSize;
	float radius;
	float _unused;
};
*/

static const float2 poisson[12] =
{
       float2(-0.326212, -0.40581),
       float2(-0.840144, -0.07358),
       float2(-0.695914, 0.457137),
       float2(-0.203345, 0.620716),
       float2(0.96234, -0.194983),
       float2(0.473434, -0.480026),
       float2(0.519456, 0.767022),
       float2(0.185461, -0.893124),
       float2(0.507431, 0.064425),
       float2(0.89642, 0.412458),
       float2(-0.32194, -0.932615),
       float2(-0.791559, -0.59771)
};

float4 PS(VS_OUTPUT input) : SV_Target
{
	const float2 uv = input.tex;

	const float2 texelSize = 1.0 / g_textureSize;

	float4 cSampleAccum = texture0.Sample(sampler0, uv);

	for(uint tap = 0; tap < 12; ++tap)
	{
		const float2 coord = uv.xy + texelSize * poisson[tap] * g_radius;

		cSampleAccum += texture0.Sample(sampler0,coord);
	}

	return cSampleAccum / 13.0;
}
