// Textures
TextureCube skyTex;

// MVP
matrix World;
matrix View;
matrix Projection;

//--------------------------------------------------------------------------------------
struct VS_INPUT
{
	float4 Pos : SV_Position;
	float3 Normal : NORMAL;
};

struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float3 TexCoord : TEXCOORD0;
};

//--------------------------------------------------------------------------------------
// DepthStates
//--------------------------------------------------------------------------------------
DepthStencilState EnableDepth
{
	DepthEnable = TRUE;
	DepthWriteMask = ZERO;
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

SamplerState sampleLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
	AddressW = Wrap;
	ComparisonFunc = NEVER;
};

//--------------------------------------------------------------------------------------
// Utilities
//--------------------------------------------------------------------------------------
inline float3 ObjectToWorldNormal(in float3 normal) {
	// Multiply by transposed inverse matrix,
	return normalize(
		World[0].xyz * normal.x +
		World[1].xyz * normal.y +
		World[2].xyz * normal.z
	);
}

inline float3 ObjectToWorldDir(in float3 dir) {
	return normalize(mul((float3x3)World, dir));
}


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input, uint id : SV_InstanceID)
{
	PS_INPUT output = (PS_INPUT)0;

	// UVs
	output.TexCoord = input.Pos.xyz;

	float4x4 modifiedView = View;
	modifiedView[3][0] = 0.0;
	modifiedView[3][1] = 0.0;
	modifiedView[3][2] = 0.0;

	// MVP multiply
	output.Pos = mul(input.Pos, World);
	output.Pos = mul(output.Pos, modifiedView);
	output.Pos = mul(output.Pos, Projection);

	return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_Target
{
	// texture lookups
	float4 texColor = skyTex.Sample(sampleLinear, input.TexCoord);
	return texColor;
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