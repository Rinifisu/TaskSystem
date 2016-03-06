
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

cbuffer vscbMesh0 : register( b0 )
{
	row_major float4x4 g_viewProjectionMatrix;
}

VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output;

	const float4 posWS = mul(input.pos, input.worldMatrix);

	output.worldPosition = posWS.xyz;

	output.pos = mul(posWS,g_viewProjectionMatrix);

	output.normal = mul(input.normal,(float3x3)input.worldMatrix);

	output.color = input.diffuseColor;

	output.tex = input.tex;

	return output;
}

//-------------------------------------------------------------

Texture2D texture0 : register( t0 );
SamplerState sampler0 : register( s0 );

cbuffer pscbMesh0 : register( b0 )
{
	float4 cameraPosition;
}

PS_OUTPUT PS(VS_OUTPUT input)
{
	PS_OUTPUT output;

	const float4 color = texture0.Sample(sampler0,input.tex);

	if (color.a < 0.5)
	{
		clip(-1);
	}

	output.color = color * input.color;

	output.depth.r = distance(cameraPosition.xyz, input.worldPosition);

	output.normal = float4(normalize(input.normal), 1);

	return output;
}
