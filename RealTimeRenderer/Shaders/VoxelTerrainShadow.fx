// Textures
// MVP
matrix World;
matrix View;
matrix Projection;

float4 InstanceData[4000];

//--------------------------------------------------------------------------------------
struct VS_INPUT
{
	float4 Pos : SV_Position;
};

struct PS_INPUT
{
	float4 Pos : SV_POSITION;
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

	// MVP multiply
	input.Pos += InstanceData[id];
	input.Pos.xyz *= InstanceData[id].w;
	output.Pos = mul(float4(input.Pos.xyz, 1.0), World);
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);

	return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_Target
{
	float depth = input.Pos.z / input.Pos.w;
	return float4(depth, depth, depth, 1.0);
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