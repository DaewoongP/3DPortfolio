#include "Shader_Client_Defines.hpp"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float g_fCamFar;
bool g_isNormal;

texture2D g_DiffuseTexture;
texture2D g_NormalTexture;
texture2D g_EmissiveTexture;

vector g_vEmissive;

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexUV : TEXCOORD0;
    float3 vTangent : TANGENT;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexUV : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));
    Out.vTexUV = In.vTexUV;
    Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.vProjPos = Out.vPosition;
    Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), g_WorldMatrix));
    Out.vBinormal = vector(normalize(cross(Out.vNormal.xyz, Out.vTangent.xyz)), 0.f);
    
	return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexUV : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
};

struct PS_OUT
{
    vector vDiffuse : SV_TARGET0;
    vector vNormal : SV_TARGET1;
    vector vDepth : SV_TARGET2;
    vector vEmissive : SV_TARGET3;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
    float3 vNormal = (float3) 0;
    
    if (true == g_isNormal)
    {
        vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
        vNormal = vNormalDesc.xyz * 2.f - 1.f;

        float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);

        vNormal = mul(vNormal, WorldMatrix);
    }
    else
        vNormal = float3(In.vNormal.xyz * 0.5f + 0.5f);

    Out.vDiffuse = vDiffuse;
    
    Out.vDiffuse.a = 1.f;

    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.f, 0.f);
    
    return Out;
}

float4 PS_MAIN_SKY(PS_IN In) : SV_TARGET0
{
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);

    return vDiffuse;
}

PS_OUT PS_MAIN_EMISSIVE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
    float3 vNormal = (float3) 0;
    
    if (true == g_isNormal)
    {
        vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
        vNormal = vNormalDesc.xyz * 2.f - 1.f;

        float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);

        vNormal = mul(vNormal, WorldMatrix);
    }
    else
        vNormal = float3(In.vNormal.xyz * 0.5f + 0.5f);

    Out.vDiffuse = vDiffuse;
    
    Out.vDiffuse.a = 1.f;

    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.f, 0.f);
    Out.vEmissive = g_vEmissive;
    
    return Out;
}

PS_OUT PS_MAIN_TEX_EMISSIVE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
    float3 vNormal = (float3) 0;
    
    vector vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexUV);
    
    vNormal = float3(In.vNormal.xyz * 0.5f + 0.5f);

    Out.vDiffuse = vDiffuse;
    Out.vDiffuse.a = 1.f;

    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.f, 0.f);
    Out.vEmissive = vEmissive;
    
    return Out;
}

technique11 DefaultTechnique
{
	pass Mesh
	{
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader	= compile vs_5_0 VS_MAIN();
		GeometryShader	= NULL /*compile gs_5_0 GS_MAIN()*/;
		HullShader		= NULL /*compile hs_5_0 HS_MAIN()*/;
		DomainShader	= NULL /*compile ds_5_0 DS_MAIN()*/;
		PixelShader		= compile ps_5_0 PS_MAIN();
	}

    pass Sky
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Depth_Disable, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_SKY();
    }

    pass EmissiveMesh
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_EMISSIVE();
    }

    pass TexEmissiveMesh
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_TEX_EMISSIVE();
    }

}