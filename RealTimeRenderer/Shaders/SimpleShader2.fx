//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------

// MVP
matrix World;
matrix View;
matrix Projection;


// LIGHT
float4 LightPos;

float4 InstanceData[512];

//--------------------------------------------------------------------------------------
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
	float4 col : TEXCOORD2;
};

struct PS_OUTPUT
{
	float4 color : SV_Target0;
	float4 normal : SV_Target1;
};


//--------------------------------------------------------------------------------------
// DepthStates
//--------------------------------------------------------------------------------------
DepthStencilState EnableDepth
{
	DepthEnable = TRUE;
	DepthWriteMask = ALL;
	DepthFunc = LESS_EQUAL;
};

BlendState NoBlending
{
	AlphaToCoverageEnable = FALSE;
	BlendEnable[0] = FALSE;
};

RasterizerState rasterizerState
{
	FillMode = SOLID;
	CullMode = BACK;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input, uint id : SV_InstanceID)
{
	PS_INPUT output = (PS_INPUT)0;

	output.Pos = mul(input.Pos, World);
	output.Pos += InstanceData[id];
	output.wPos = output.Pos.xyz;
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);
	//output.Norm = mul(float4(input.Norm, 1), World).xyz;
	output.Norm = input.Norm;
	output.col = input.Col;

	return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
PS_OUTPUT PS(PS_INPUT input) : SV_Target
{
	PS_OUTPUT output = (PS_OUTPUT)0;

	float3 lightDir = normalize(LightPos.xyz - input.wPos);
	float diff = max(0.2, dot(lightDir.xyz, input.Norm));

	//return float4(diff, diff, diff, 1);
	output.color = input.col * diff;
	output.normal = float4(input.Norm * 0.5 + 0.5, 1);
	return output;
}


//--------------------------------------------------------------------------------------
// Technique
//--------------------------------------------------------------------------------------
technique11 Render
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_4_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PS()));

		SetDepthStencilState(EnableDepth, 0);
		SetRasterizerState(rasterizerState);
		SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
	}
}