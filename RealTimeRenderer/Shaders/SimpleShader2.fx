// Textures
Texture2D diffuseTex;
Texture2D normal;

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
	float2 TexCoord : TEXCOORD;
	float3 Normal : NORMAL;
	float3 Binormal : BINORMAL;
	float3 Tangent : TANGENT;
	float4 Color : COLOR;
};

struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float2 TexCoord : TEXCOORD0;
	float3 wPos : TEXCOORD1;
	float3 Normal : TEXCOORD2;
	float4 Color : TEXCOORD3;
};

struct PS_OUTPUT
{
	float4 color : SV_Target0;
	float3 normal : SV_Target1;
	float3 position : SV_Target2;
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

SamplerState sampleLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};



//--------------------------------------------------------------------------------------
// Utilities
//--------------------------------------------------------------------------------------
inline float3 ObjectToWorldNormal(in float3 norm) {
	// Multiply by transposed inverse matrix,
	return normalize(
		World[0].xyz * norm.x +
		World[1].xyz * norm.y +
		World[2].xyz * norm.z
	);
}


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input, uint id : SV_InstanceID)
{
	PS_INPUT output = (PS_INPUT)0;

	output.Pos = mul(input.Pos, World);
	//output.Pos += InstanceData[id];
	output.wPos = output.Pos.xyz;
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);

	// UVs
	output.TexCoord = input.TexCoord;

	// normal in WS
	output.Normal = ObjectToWorldNormal(input.Normal);
	output.Color = input.Color;

	return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
PS_OUTPUT PS(PS_INPUT input) : SV_Target
{
	PS_OUTPUT output = (PS_OUTPUT)0;

	float4 texColor = diffuseTex.Sample(sampleLinear, input.TexCoord);
	float3 lightDir = normalize(LightPos.xyz - input.wPos);
	float diff = max(0.2, dot(lightDir.xyz, input.Normal));

	//return float4(diff, diff, diff, 1);
	output.color = input.Color * diff * texColor;
	output.normal = float4(input.Normal * 0.5 + 0.5, 1);
	output.position = input.wPos;
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