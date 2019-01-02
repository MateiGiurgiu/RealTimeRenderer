cbuffer ConstantBuffer : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
	float4 LightPos;
}


struct VS_INPUT
{
	float4 Pos : SV_Position;
	float2 Tex : TEXCOORD;
	float3 Norm : NORMAL;
	float3 Tang : TANGENT;
	float4 Col : COLOR;
};

struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float3 wPos : TEXCOORD0;
	float3 Norm : TEXCOORD1;
};

PS_INPUT main(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;

	output.Pos = mul(input.Pos, World);
	output.wPos = output.Pos.xyz;
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);
	output.Norm = mul(float4(input.Norm, 1), World).xyz;

	return output;
}