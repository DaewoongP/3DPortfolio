float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

struct VS_IN
{
	float3 vPosition : POSITION;
	float3 vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4 vPosition : SV_POSITION;
	float3 vTexUV : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT) 0;
    
    float4x4 WVMatrix = mul(g_WorldMatrix, g_ViewMatrix);
    float4x4 WVPMatrix = mul(WVMatrix, g_ProjMatrix);
    Out.vPosition = mul(vector(In.vPosition, 1.f), WVPMatrix);

	return Out;
}

struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float3 vTexUV : TEXCOORD0;
};

float4 PS_MAIN(PS_IN In) : SV_TARGET0
{
    float4 vColor = (float4) 0;
    vColor = float4(0.f, 1.f, 0.f, 1.f);
	return vColor;
}

technique11 DefaultTechnique
{
	pass Axis
	{
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
		HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
		DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}