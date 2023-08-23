

/* Sampler State */
sampler LinearSampler = sampler_state
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

sampler PointSampler = sampler_state
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = WRAP;
	AddressV = WRAP;
};

/* Raterizer State */
RasterizerState RS_Default
{
	FillMode = Solid;
	CullMode = Back;
	FrontCounterClockwise = false;
};

RasterizerState RS_Cull_CW
{
	FillMode = Solid;
	CullMode = front;
	FrontCounterClockwise = false;
};

RasterizerState RS_Cull_None
{
	FillMode = Solid;
	CullMode = None;
	FrontCounterClockwise = false;
};

/* Depth_Stencil State */

DepthStencilState DSS_Default
{
	DepthEnable = true;
	DepthWriteMask = all;
	DepthFunc = less_equal;
};

DepthStencilState DSS_Depth_Disable
{
	DepthEnable = false;
	DepthWriteMask = zero;
};

/* Blend State */
BlendState BS_Default
{
	BlendEnable[0] = false;
};

BlendState BS_AlphaBlend
{
	BlendEnable[0] = true;

	SrcBlend = Src_Alpha;
	DestBlend = Inv_Src_Alpha;
	BlendOp = Add;
};

////////////////////////////////////Roughness
float D_GGX(float NdotH, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH2 = NdotH * NdotH;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	return a2 / (3.14 * denom * denom);
}

float G_GGX(float NdotL, float NdotV, float roughness)
{
	float k = roughness * roughness / 2.0;
	float G1_L = NdotV / (NdotV * (1.0 - k) + k);
	float G1_V = NdotL / (NdotL * (1.0 - k) + k);
	return G1_L * G1_V;
}

float F_Schlick(float VdotH, float F0)
{
	return F0 + (1.0 - F0) * pow(1.0 - VdotH, 5.0);
}

float3 CalculateSpecular(float3 lightDir, float3 viewDir, float3 normal, float roughness, float F0)
{
	float3 H = normalize(lightDir + viewDir);
	float NdotL = saturate(dot(normal, lightDir));
	float NdotV = saturate(dot(normal, viewDir));
	float NdotH = saturate(dot(normal, H));
	float VdotH = saturate(dot(viewDir, H));

	float D = D_GGX(NdotH, roughness);
	float G = G_GGX(NdotL, NdotV, roughness);
	float F = F_Schlick(VdotH, F0);

	float3 specularColor = (D * G * F) / (4.0 * NdotL * NdotV + 0.001);
	return specularColor;
}