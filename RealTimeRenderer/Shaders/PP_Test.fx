// G buffers
Texture2D bufferColor;
Texture2D bufferNormal;
Texture2D bufferPosition;
Texture2D shadowMap;

// Light Information
float3 LightDir1;

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
// Pixel Shader - Visualize Albedo
//--------------------------------------------------------------------------------------
float4 PS0(PS_INPUT input) : SV_Target
{
	float4 texColor = bufferColor.Sample(sampleLinear, input.TexCoord);
	return texColor;
}

//--------------------------------------------------------------------------------------
// Pixel Shader - Visualize Normals
//--------------------------------------------------------------------------------------
float4 PS1(PS_INPUT input) : SV_Target
{
	float3 normal = bufferNormal.Sample(sampleLinear, input.TexCoord).rgb;
	return float4(normal * 0.5 + 0.5, 1);
}

//--------------------------------------------------------------------------------------
// Pixel Shader - Visualize Position
//--------------------------------------------------------------------------------------
float4 PS2(PS_INPUT input) : SV_Target
{
	float4 texColor = bufferPosition.Sample(sampleLinear, input.TexCoord);
	return texColor;
}

//--------------------------------------------------------------------------------------
// Pixel Shader - Visualize ShadowMap
//--------------------------------------------------------------------------------------
float4 PS3(PS_INPUT input) : SV_Target
{
	float4 texColor = shadowMap.Sample(sampleLinear, input.TexCoord);
	return texColor;
}

//--------------------------------------------------------------------------------------
// Pixel Shader - Light
//--------------------------------------------------------------------------------------
float4 PSLight(PS_INPUT input) : SV_Target
{
	float3 color = bufferColor.Sample(sampleLinear, input.TexCoord).rgb;
	float3 normal = bufferNormal.Sample(sampleLinear, input.TexCoord).xyz;

	float light = max(0.0, dot(normal, -LightDir1));

	return float4(light * color, 1);
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
		SetPixelShader(CompileShader(ps_4_0, PS0()));

		SetDepthStencilState(DisableDepth, 0);
		SetRasterizerState(rasterizerState);
		SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
	}

	pass P1
	{
		SetVertexShader(CompileShader(vs_4_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PS1()));

		SetDepthStencilState(DisableDepth, 0);
		SetRasterizerState(rasterizerState);
		SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
	}

	pass P2
	{
		SetVertexShader(CompileShader(vs_4_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PS2()));

		SetDepthStencilState(DisableDepth, 0);
		SetRasterizerState(rasterizerState);
		SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
	}

	pass P3
	{
		SetVertexShader(CompileShader(vs_4_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PS3()));

		SetDepthStencilState(EnableDepth, 0);
		SetRasterizerState(rasterizerState);
		SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
	}

	pass P4
	{
		SetVertexShader(CompileShader(vs_4_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PSLight()));

		SetDepthStencilState(DisableDepth, 0);
		SetRasterizerState(rasterizerState);
		SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
	}
}