// Textures
Texture2D diffuseTex;

// MVP
matrix World;
matrix View;
matrix Projection;

float Time;
float LifeTime;

//--------------------------------------------------------------------------------------
struct VS_INPUT
{
	float4 Pos : SV_Position;
	float2 TexCoord : TEXCOORD;
	float3 Normal : NORMAL;
};

struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float2 TexCoord : TEXCOORD0;
	float3 Normal : TEXCOORD1;
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

BlendState AlphaBlending
{
	BlendEnable[0] = TRUE;
	SrcBlend = SRC_ALPHA;
	DestBlend = ONE;     
	BlendOp = ADD;     
	SrcBlendAlpha = ZERO;     
	DestBlendAlpha = ZERO;     
	BlendOpAlpha = ADD;     
	RenderTargetWriteMask[0] = 0x0F;
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

float rand_1_05(in float2 uv)
{
	float2 noise = (frac(sin(dot(uv, float2(12.9898, 78.233)*2.0)) * 43758.5453));
	return abs(noise.x + noise.y) * 0.5;
}


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input, uint id : SV_InstanceID)
{
	PS_INPUT output = (PS_INPUT)0;

	//input.Pos += InstanceData[id];
	//input.Pos.xyz *= InstanceData[id].w;

	float3 particlePos = float3(0,0,0);

	float random = rand_1_05(float(id + 1));

	float3 randPoint = float3
		(
			sin(random * 538),
			fmod(id, 2) == 0? 1.0 : -1.0 * random * 0.5,
			cos(random * 798)
			);
	randPoint = normalize(randPoint);

	float t = fmod(Time, 2) / 2;

	float scale = t * 1.1 + abs(sin(Time)) * 0.5;
	input.Pos.xy *= scale + random;

	particlePos = randPoint * t;

	//particlePos.z = 2 * t * sin((float(id + 1) / 10));
	//particlePos.x = 2 * t * cos((float(id + 1) / 10));
	//particlePos.y = 2 * t* sin(float(id + 1) / 10);

	//particlePos.y = sin(id);


	// BILBOARDING
	float4x4 viewInv = transpose(View);
	float3 Vx = viewInv[0].xyz;     
	float3 Vy = -viewInv[1].xyz;
	//particlePos.xyz = (particlePos.x * Vx + particlePos.y * Vy).xyz;

	// MVP & bilboarding
	output.Pos = mul(float4(particlePos.xyz, 1.0), World);
	output.Pos = mul(output.Pos, View);
	output.Pos -= float4(input.Pos.x, -input.Pos.y, 0.0, 0.0);
	output.Pos = mul(output.Pos, Projection);


	// UVs
	output.TexCoord = input.TexCoord;

	// normal in WS
	output.Normal = ObjectToWorldDir(input.Normal);

	return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_Target
{
	// texture lookups
	float4 texColor = diffuseTex.Sample(sampleLinear, input.TexCoord);

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
		SetBlendState(AlphaBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
	}
}