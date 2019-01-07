// Textures
Texture2D diffuseTex;
Texture2D normalTex;
Texture2D specularTex;

// MVP
matrix World;
matrix View;
matrix Projection;

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
	float3 Binormal : TEXCOORD3;
	float3 Tangent : TEXCOORD4;
	float4 Color : TEXCOORD5;
};

struct PS_OUTPUT
{
	float4 color : SV_Target0;
	float4 normal : SV_Target1;
	float4 position : SV_Target2;
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
inline float3 ObjectToWorldDir(in float3 normal) {
	// Multiply by transposed inverse matrix,
	return normalize(
		World[0].xyz * normal.x +
		World[1].xyz * normal.y +
		World[2].xyz * normal.z
	);
}


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;

	output.Pos = mul(input.Pos, World);
	output.wPos = output.Pos.xyz;
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);

	// UVs
	output.TexCoord = input.TexCoord;

	// normal, binormal, tangent in WS
	output.Normal = ObjectToWorldDir(input.Normal);
	output.Binormal = -ObjectToWorldDir(input.Binormal);
	output.Tangent = ObjectToWorldDir(input.Tangent);

	output.Color = input.Color;

	return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
PS_OUTPUT PS(PS_INPUT input) : SV_Target
{
	PS_OUTPUT output = (PS_OUTPUT)0;

	// texture lookups
	float3 diffuse = diffuseTex.Sample(sampleLinear, input.TexCoord).rgb;
	float specular = specularTex.Sample(sampleLinear, input.TexCoord).r;
	float4 normal = normalTex.Sample(sampleLinear, input.TexCoord);

	float3 finalNormal;
	if (length(normal.rgb) > 0.05)
	{
		float3 N = normalize(2.0 * normal.xyz - 1.0);

		finalNormal = normalize(
			N.x * input.Tangent +
			N.y * input.Binormal +
			N.z * input.Normal
		);
	}
	else
	{
		finalNormal = input.Normal;
	}
	
	if (length(diffuse) > 0.01)
	{
		output.color.rgb = diffuse;
	}
	else
	{
		output.color.rgb = input.Color.rgb;
	}
	output.color.a = specular;
	output.normal = float4(finalNormal, 1);
	output.position = float4(input.wPos, 1);
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