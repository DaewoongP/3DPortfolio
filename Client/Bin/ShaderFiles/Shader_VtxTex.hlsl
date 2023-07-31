#include "Shader_Client_Defines.hpp"

Texture2D g_Texture;
float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

float4 g_vColor;
float g_fUVPercent;

float g_fSize;
float g_fTimeAcc;

struct VS_IN
{
	float3 vPosition : POSITION;
	float2 vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT) 0;
	
    float4x4 WVMatrix = mul(g_WorldMatrix, g_ViewMatrix);
    float4x4 WVPMatrix = mul(WVMatrix, g_ProjMatrix);

    Out.vPosition = mul(vector(In.vPosition, 1.f), WVPMatrix);
    Out.vTexUV = In.vTexUV;
	
	return Out;
}

struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD0;
};

float4 PS_MAIN(PS_IN In) : SV_TARGET0
{
	float4 vColor = (float4) 0;
	
    vColor = g_Texture.Sample(LinearSampler, In.vTexUV);

	return vColor;
}

float4 PS_MAIN_UI(PS_IN In) : SV_TARGET0
{
    float4 vColor = (float4) 0;
	
    vColor = g_Texture.Sample(PointSampler, In.vTexUV);
	
    //if (0.1f >= vColor.a)
    //    discard;

    return vColor;
}

float4 PS_MAIN_COLOR_UI(PS_IN In) : SV_TARGET0
{
    float4 vColor = (float4) 0;
	
    vColor = g_Texture.Sample(PointSampler, In.vTexUV);
	
	// 알파값은 그대로 유지.
    if (0.1f < vColor.a)
        vColor.xyz = g_vColor.xyz;
	else
        discard;
	
    return vColor;
}

float4 PS_MAIN_SIZE_UI(PS_IN In) : SV_TARGET0
{
    float4 vColor = (float4) 0;
    
    vColor = g_Texture.Sample(PointSampler, In.vTexUV);

    if (g_fUVPercent < In.vTexUV.x)
        discard;
    
    return vColor;
}

float4 PS_MAIN_VERTICAL_COLOR(PS_IN In) : SV_TARGET0
{
    float4 vColor = (float4) 0;
    
    vColor = g_Texture.Sample(PointSampler, In.vTexUV);

    if (g_fUVPercent > 1.f - In.vTexUV.y && 0.2f < vColor.r)
        vColor = g_vColor;
    
    return vColor;
}

float4 PS_MAIN_TIME_COLOR(PS_IN In) : SV_TARGET0
{
    float4 vColor = (float4) 0;
    
    vColor = g_Texture.Sample(LinearSampler, In.vTexUV);
    
    if (0.1f > vColor.a)
        discard;

    // -1 ~ 1 정규화
    float2 vPos = float2(In.vTexUV.x * 2.f - 1.f, In.vTexUV.y * 2.f - 1.f);
    
    // 빨간색 반지름 범위
    float fLength = sqrt(vPos.x * vPos.x + vPos.y * vPos.y);
    
    if (g_fTimeAcc / 3.f > fLength)
        vColor = float4(1.f, 0.f, 0.f, 1.f);
    else
        vColor = float4(0.5f, 0.1f, 0.1f, 0.5f);
    
    return vColor;
}

float4 PS_MAIN_TRAIL(PS_IN In) : SV_TARGET0
{
    float4 vColor = (float4) 0;
	
    vColor = g_Texture.Sample(LinearSampler, In.vTexUV);
	
    vColor = float4(1.f, 1.f, 1.f, 1.f);
    
    return vColor;
}

technique11 DefaultTechnique
{
	pass BackGround
	{
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader	= compile vs_5_0 VS_MAIN();
		GeometryShader	= NULL /*compile gs_5_0 GS_MAIN()*/;
		HullShader		= NULL /*compile hs_5_0 HS_MAIN()*/;
		DomainShader	= NULL /*compile ds_5_0 DS_MAIN()*/;
		PixelShader		= compile ps_5_0 PS_MAIN();
	}

	pass UI
	{
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader	= compile vs_5_0 VS_MAIN();
		GeometryShader	= NULL /*compile gs_5_0 GS_MAIN()*/;
		HullShader		= NULL /*compile hs_5_0 HS_MAIN()*/;
		DomainShader	= NULL /*compile ds_5_0 DS_MAIN()*/;
		PixelShader		= compile ps_5_0 PS_MAIN_UI();
	}

    pass ColorUI
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_COLOR_UI();
    }

    pass NonBlendUI
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_UI();
    }

    pass SizeUI
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_SIZE_UI();
    }

    pass VerticalColorUI
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_VERTICAL_COLOR();
    }

    pass TimeColor
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_TIME_COLOR();
    }
// 7
    pass Trail
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_TRAIL();
    }

	pass Effect
	{
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader	= compile vs_5_0 VS_MAIN();
		GeometryShader	= NULL /*compile gs_5_0 GS_MAIN()*/;
		HullShader		= NULL /*compile hs_5_0 HS_MAIN()*/;
		DomainShader	= NULL /*compile ds_5_0 DS_MAIN()*/;
		PixelShader		= compile ps_5_0 PS_MAIN();
	}
}