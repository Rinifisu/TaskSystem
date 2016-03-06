// Reserved
cbuffer vscbSprite : register(b0)
{
	row_major float2x4 g_transform;
};

// Customizable
cbuffer vsConstants1 : register(b1)
{
	float4 g_param;
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float4 color : COLOR0;
};

VS_OUTPUT VS(uint id: SV_VERTEXID)
{
	const uint particleIndex = id / 4;
	const uint vertexInQuad = id % 4;

	//
	// vertexInQuad
	//
	// 0 ---- 1
	// |      |
	// |      |
	// 2 ---- 3
	//

	VS_OUTPUT output;

	float2 position;
	position.x = particleIndex * 5 + ((vertexInQuad % 2) ? 5.0 : 0.0);
	position.y = (vertexInQuad & 2) ? 480.0 : 300.0 + sin(position.x * g_param.x + g_param.z) * g_param.y;

	////////////////////////
	//
	// Siv3D Default 2D transfom
	//
	output.position.xy = g_transform._13_14 + position.x * g_transform._11_12 + position.y * g_transform._21_22;
	output.position.z = g_transform._23;
	output.position.w = g_transform._24;
	//
	////////////////////////

	//
	// Texture UV
	//
	output.tex.x = (vertexInQuad % 2) ? 1.0 : 0.0;
	output.tex.y = (vertexInQuad & 2) ? 1.0 : 0.0;

	//
	// Color
	//
	output.color = float4(0.1, 0.7, 1, 0.7);

	return output;
}
