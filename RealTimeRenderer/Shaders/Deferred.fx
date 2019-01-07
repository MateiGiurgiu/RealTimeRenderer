// G buffers
Texture2D bufferColor;
Texture2D bufferNormal;
Texture2D bufferPosition;
Texture2D shadowMap;

// other textures
Texture2D toonRamp;

// Light Information
float3 ViewDir;
float3 AmbientColor;


float4x4 DirectionalLightView;
float4x4 DirectionalLightProj;
float3 DirectionalLightPos;
float3 DirectionalLightDir;
float4 DirectionalLightColor;

//--------------------------------------------------------------------------------------
// Simple Input
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

SamplerState sampleShadow
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = BORDER;
	AddressV = BORDER;
	BorderColor = float4(1, 1, 1, 1);
};


//--------------------------------------------------------------------------------------
// Vertex Shader - Simple
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
// Pixel Shader - Visualize Specular
//--------------------------------------------------------------------------------------
float4 PS1(PS_INPUT input) : SV_Target
{
	float specular = bufferColor.Sample(sampleLinear, input.TexCoord).w;
	return float4(specular, specular, specular, 1.0f);
}

//--------------------------------------------------------------------------------------
// Pixel Shader - Visualize Normals
//--------------------------------------------------------------------------------------
float4 PS2(PS_INPUT input) : SV_Target
{
	float3 normal = bufferNormal.Sample(sampleLinear, input.TexCoord).rgb;
	return float4(normal * 0.5 + 0.5, 1);
}

//--------------------------------------------------------------------------------------
// Pixel Shader - Visualize Position
//--------------------------------------------------------------------------------------
float4 PS3(PS_INPUT input) : SV_Target
{
	float4 texColor = bufferPosition.Sample(sampleLinear, input.TexCoord);
	return texColor;
}

//--------------------------------------------------------------------------------------
// Pixel Shader - Visualize ShadowMap
//--------------------------------------------------------------------------------------
float4 PS4(PS_INPUT input) : SV_Target
{
	float4 texColor = shadowMap.Sample(sampleLinear, input.TexCoord);
	return texColor;
}

//--------------------------------------------------------------------------------------
// Pixel Shader - Light
//--------------------------------------------------------------------------------------
inline float ShadowAttenuation(float3 pos, float nDotL)
{
	// transfrom in light's space
	float4 lightViewPosition = float4(pos, 1.0);
	lightViewPosition = mul(lightViewPosition, DirectionalLightView);
	lightViewPosition = mul(lightViewPosition, DirectionalLightProj);

	float2 projCoords;
	projCoords.x = lightViewPosition.x / lightViewPosition.w * 0.5 + 0.5f;
	projCoords.y = -lightViewPosition.y / lightViewPosition.w * 0.5 + 0.5f;

	float sampledDepth = shadowMap.Sample(sampleShadow, projCoords).r;
	float currentDepth = lightViewPosition.z / lightViewPosition.w;

	float bias = 0.001;
	float shadow = currentDepth - bias > sampledDepth ? 1.0 : 0.0;
	if (currentDepth > 1)
		shadow = 0.0;
	return shadow;
}

float4 PSLight(PS_INPUT input) : SV_Target
{
	float4 color = bufferColor.Sample(sampleLinear, input.TexCoord);
	float3 normal = bufferNormal.Sample(sampleLinear, input.TexCoord).xyz;
	float3 position = bufferPosition.Sample(sampleLinear, input.TexCoord).xyz;
	
	// the infamous n dot l
	float nDotL = max(0.0, dot(normal, -DirectionalLightDir));
	float nDotH = max(0.0, dot(normal, -normalize(DirectionalLightDir + ViewDir)));

	// ambient
	float3 ambient = 0.35 * AmbientColor.rgb;

	// difuse
	float3 diffuse = nDotL * DirectionalLightColor.rgb * DirectionalLightColor.a;

	// specular
	float specularPower = color.w * 80;
	float3 specular = (specularPower > 0.01 ? pow(nDotH, specularPower) * DirectionalLightColor.rgb : float3(0,0,0)) * DirectionalLightColor.a;
	
	// shadow
	float shadow = ShadowAttenuation(position, nDotL);

	// final lighting
	float3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color.rgb;

	return float4(lighting, 1.0f);
}


//--------------------------------------------------------------------------------------
// Pixel Shader - Toon Light
//--------------------------------------------------------------------------------------
float4 PSToonLight(PS_INPUT input) : SV_Target
{
	float4 color = bufferColor.Sample(sampleLinear, input.TexCoord);
	float3 normal = bufferNormal.Sample(sampleLinear, input.TexCoord).xyz;
	float3 position = bufferPosition.Sample(sampleLinear, input.TexCoord).xyz;

	// the infamous n dot l
	float nDotL = max(0.0, dot(normal, -DirectionalLightDir));
	nDotL = toonRamp.Sample(sampleLinear, float2(nDotL, 0.5));
	float nDotH = max(0.0, dot(normal, -normalize(DirectionalLightDir + ViewDir)));
	//nDotH = toonRamp.Sample(sampleLinear, float2(nDotH, 0.5));

	// ambient
	float3 ambient = 0.35 * AmbientColor.rgb;

	// difuse
	float3 diffuse = nDotL * DirectionalLightColor.rgb * DirectionalLightColor.a;

	// specular
	float specularPower = color.w * 80;
	float3 specular = (specularPower > 0.01 ? pow(nDotH, specularPower) * DirectionalLightColor.rgb : float3(0,0,0)) * DirectionalLightColor.a;

	// shadow
	float shadow = ShadowAttenuation(position, nDotL);

	// final lighting
	float3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color.rgb;

	return float4(lighting, 1.0f);
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

		SetDepthStencilState(DisableDepth, 0);
		SetRasterizerState(rasterizerState);
		SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
	}

	pass P4
	{
		SetVertexShader(CompileShader(vs_4_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PS4()));

		SetDepthStencilState(EnableDepth, 0);
		SetRasterizerState(rasterizerState);
		SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
	}

	pass P5
	{
		SetVertexShader(CompileShader(vs_4_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PSLight()));

		SetDepthStencilState(DisableDepth, 0);
		SetRasterizerState(rasterizerState);
		SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
	}

	pass P6
	{
		SetVertexShader(CompileShader(vs_4_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PSToonLight()));

		SetDepthStencilState(DisableDepth, 0);
		SetRasterizerState(rasterizerState);
		SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
	}
}