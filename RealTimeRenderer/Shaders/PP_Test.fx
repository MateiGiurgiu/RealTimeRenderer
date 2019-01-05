matrix Projection;
Texture2D buffer1;
Texture2D buffer2;
Texture2D buffer3;

//--------------------------------------------------------------------------------------
struct VS_INPUT
{
	float4 Pos : SV_Position;
	float2 TexCoord : TEXCOORD;
};

struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float2 TexCoord : TEXCOORD0;
};


//--------------------------------------------------------------------------------------
// DepthStates
//--------------------------------------------------------------------------------------
DepthStencilState DisableDepth
{
	DepthEnable = FALSE;
};

BlendState NoBlending
{
	AlphaToCoverageEnable = FALSE;
	BlendEnable[0] = FALSE;
};

RasterizerState rasterizerState
{
	FillMode = SOLID;
	CullMode = NONE;
};

SamplerState sampleLinear 
{ 
	Filter = MIN_MAG_MIP_LINEAR;     
	AddressU = Wrap;     
	AddressV = Wrap; 
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input, uint id : SV_InstanceID)
{
	PS_INPUT output = (PS_INPUT)0;

	output.Pos = input.Pos;
	output.TexCoord = float2(input.TexCoord.x, input.TexCoord.y * -1);

	return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader - Visualize Buffer 1
//--------------------------------------------------------------------------------------
float4 PS1(PS_INPUT input) : SV_Target
{
	float4 texColor = buffer1.Sample(sampleLinear, input.TexCoord);
	return texColor;
}

//--------------------------------------------------------------------------------------
// Pixel Shader - Visualize Buffer 2
//--------------------------------------------------------------------------------------
float4 PS2(PS_INPUT input) : SV_Target
{
	float4 texColor = buffer2.Sample(sampleLinear, input.TexCoord);
	return texColor;
}

//--------------------------------------------------------------------------------------
// Pixel Shader - Visualize Buffer 3
//--------------------------------------------------------------------------------------
float4 PS3(PS_INPUT input) : SV_Target
{
	float4 texColor = buffer3.Sample(sampleLinear, input.TexCoord);
	if (texColor.g < 0)
	{
		return float4(0, 1, 0, 1);
	}
	else
	{
		return float4(1, 0, 0, 1);
	}
	//return texColor;
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
		SetPixelShader(CompileShader(ps_4_0, PS1()));

		SetDepthStencilState(DisableDepth, 0);
		SetRasterizerState(rasterizerState);
		SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
	}

	pass P1
	{
		SetVertexShader(CompileShader(vs_4_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PS2()));

		SetDepthStencilState(DisableDepth, 0);
		SetRasterizerState(rasterizerState);
		SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
	}

	pass P2
	{
		SetVertexShader(CompileShader(vs_4_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PS3()));

		SetDepthStencilState(DisableDepth, 0);
		SetRasterizerState(rasterizerState);
		SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
	}
}