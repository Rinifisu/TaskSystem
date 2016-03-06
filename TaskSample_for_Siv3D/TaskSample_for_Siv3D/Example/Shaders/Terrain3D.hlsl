
struct VS_INPUT
{
	float4 pos : POSITION;
	float3 normal : NORMAL;
	float2 tex : TEXCOORD0;
	row_major float4x4 worldMatrix : MATRIX;
	float4 diffuseColor : COLOR;
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float3 normal : TEXCOORD0;
	float3 worldPosition : TEXCOORD1;
	float4 color : TEXCOORD2;
	float2 tex : TEXCOORD3;
};

struct PS_OUTPUT
{
	float4 color : SV_Target0;
	float  depth : SV_Target1;
	float4 normal : SV_Target2;
};

//-------------------------------------------------------------

Texture2D texture0 : register( t0 );
SamplerState sampler0 : register( s0 );

cbuffer vscbMesh0 : register( b0 )
{
	row_major float4x4 g_viewProjectionMatrix;
}

VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output;

	const float hScale = 5.0;
	const float dUV = 1.0 / 128;
	const float n = texture0.SampleLevel(sampler0,input.tex + float2(0, -dUV), 0).r;
	const float s = texture0.SampleLevel(sampler0,input.tex + float2(0, dUV), 0).r;
	const float w = texture0.SampleLevel(sampler0,input.tex + float2(-dUV, 0), 0).r;
	const float e = texture0.SampleLevel(sampler0,input.tex + float2(dUV, 0), 0).r;
	const float h = texture0.SampleLevel(sampler0,input.tex + float2(0, 0), 0).r;
	const float d = 160.0 / 128 / hScale;

	input.pos.y *= h * hScale;

	const float4 posWS = mul(input.pos, input.worldMatrix);

	output.pos = mul(posWS,g_viewProjectionMatrix);

	output.normal = normalize(float3(w-e, 2*d, s-n));

	output.worldPosition = posWS.xyz;

	output.color = input.diffuseColor;

	output.tex = input.tex;

	return output;
}

//-------------------------------------------------------------


cbuffer pscbMesh0 : register( b0 )
{
	float3 g_cameraPosition;
	uint g_fogType;
	float4 g_fogParam;
	float4 g_fogColor;
}

PS_OUTPUT PS(VS_OUTPUT input)
{
	PS_OUTPUT output;

	const float4 color = texture0.Sample(sampler0, input.tex * 8);

	if (color.a < 0.5)
	{
		clip(-1);
	}

	output.color = color * input.color;

	output.depth.r = distance(g_cameraPosition.xyz, input.worldPosition);

	output.normal = float4(normalize(input.normal), 1);

	return output;
}
