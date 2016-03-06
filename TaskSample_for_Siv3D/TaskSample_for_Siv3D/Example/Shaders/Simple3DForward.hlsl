
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
	float3 worldPosition : TEXCOORD0;
	float4 color : TEXCOORD1;
	float2 tex : TEXCOORD2;
};

//-------------------------------------------------------------

struct Light
{
	float3 position;
	uint type;
	float4 diffuseColor;
	float4 attenuation;
};

cbuffer vscbMesh0 : register( b0 )
{
	row_major float4x4 g_viewProjectionMatrix;

	float4 g_ambientColor;

	Light g_lights[4];
}

float CalculateDirectionalLight(float3 surfaceNormal, float3 direction)
{
	return saturate(dot(surfaceNormal, direction.xyz)); 
}

float CalculatePointLight(float3 surfaceNormal, float3 surfacePosition, float3 lightPosition, float3 lightAttenuation)
{
	float3 lightDirection = (lightPosition - surfacePosition);
	const float d = length(lightDirection);
	const float Kc = lightAttenuation.x;
	const float Kl = lightAttenuation.y;
	const float Kq = lightAttenuation.z;
	const float f_att = 1.0/(Kc+Kl*d+Kq*d*d);
	lightDirection = normalize(lightDirection);
	const float diffuseInfluence = saturate(dot(lightDirection,surfaceNormal)) * f_att;
	return diffuseInfluence;
}

VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output;

	const float4 posWS = mul(input.pos, input.worldMatrix);

	output.worldPosition = posWS.xyz;

	output.pos = mul(posWS,g_viewProjectionMatrix);

	output.tex = input.tex;

	const float3 surfaceNormal = normalize(mul(input.normal,(float3x3)input.worldMatrix));

	output.color = g_ambientColor;

	[unroll]
	for(uint i = 0; i < 4; ++i)
	{
		output.color.rgb += 
		(g_lights[i].type ? 
		CalculatePointLight(surfaceNormal, output.worldPosition, g_lights[i].position.xyz, g_lights[i].attenuation.xyz)
		: CalculateDirectionalLight(surfaceNormal, g_lights[i].position.xyz)) * g_lights[i].diffuseColor.rgb;
	}

	output.color *= input.diffuseColor;

	return output;
}

//-------------------------------------------------------------

Texture2D texture0 : register( t0 );
SamplerState sampler0 : register( s0 );

cbuffer pscbMesh0 : register( b0 )
{
	float3 g_cameraPosition;
	uint g_fogType;
	float4 g_fogParam;
	float4 g_fogColor;
}

float4 PS(VS_OUTPUT input) : SV_Target
{
	return texture0.Sample(sampler0,input.tex) * input.color;
}
