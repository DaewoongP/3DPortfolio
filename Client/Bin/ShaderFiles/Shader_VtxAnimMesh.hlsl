#include "Shader_Client_Defines.hpp"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float4x4 g_BoneMatrices[256];
texture2D g_DiffuseTexture;
texture2D g_NormalTexture;
texture2D g_EmissiveTexture;
texture2D g_DissolveTexture;

float g_fCamFar;
float4 g_vCamPos;

float g_fRimWidth;

float g_fThickness, g_fDissolveTimeAcc;

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexUV : TEXCOORD0;
    float3 vTangent : TANGENT;
    uint4  vBlendIndices : BLENDINDEX;
    float4 vBlendWeights : BLENDWEIGHT;
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

    float fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);

    matrix BoneMatrix = g_BoneMatrices[In.vBlendIndices.x] * In.vBlendWeights.x +
		g_BoneMatrices[In.vBlendIndices.y] * In.vBlendWeights.y +
		g_BoneMatrices[In.vBlendIndices.z] * In.vBlendWeights.z +
		g_BoneMatrices[In.vBlendIndices.w] * fWeightW;

    vector vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
    vector vNormal = mul(vector(In.vNormal, 0.f), BoneMatrix);
	
    Out.vPosition = mul(vPosition, matWVP);
    Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
    Out.vTexUV = In.vTexUV;
    Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    // SV_POSITION���� ���������� ������ �� -> ���������̽����� �����ǰ� ����.
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
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
    float4 vEmissive : SV_TARGET3;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
    
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
    // �ؽ�ó�� �븻���� -1~1�� ����� ���ϱ⶧���� 0~1�� ����ȭ�Ǿ� �ִ�. ���� ���������� ��ȯ�������.
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);

    vNormal = mul(vNormal, WorldMatrix);

    Out.vDiffuse = vDiffuse;
	
    // UNORM 4�� Ÿ�Կ� ���� �����Ƿ� ���⼭ 0~1�� ����ó���ϰ� ���߿� ������ -1~1�� ����ó���� �ٽ��Ѵ�.
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    // SV_POSITION���� �������� �ʾҴ� ���������� ���̹Ƿ� w�����⸦ ������ z�� (���������̽�) ���� r, 
    // �ٽ� ���� ���̴����� w�� �����ֱ� ���� b�� ���� �ٽ� ��������.
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.f, 0.f);
    
    return Out;
}

PS_OUT PS_MAIN_EMISSIVE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
    vector vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexUV);
    // �ؽ�ó�� �븻���� -1~1�� ����� ���ϱ⶧���� 0~1�� ����ȭ�Ǿ� �ִ�. ���� ���������� ��ȯ�������.
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);

    vNormal = mul(vNormal, WorldMatrix);

    Out.vDiffuse = vDiffuse;
	
    // UNORM 4�� Ÿ�Կ� ���� �����Ƿ� ���⼭ 0~1�� ����ó���ϰ� ���߿� ������ -1~1�� ����ó���� �ٽ��Ѵ�.
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    
    Out.vEmissive = vEmissive;
    
    // SV_POSITION���� �������� �ʾҴ� ���������� ���̹Ƿ� w�����⸦ ������ z�� (���������̽�) ���� r, 
    // �ٽ� ���� ���̴����� w�� �����ֱ� ���� b�� ���� �ٽ� ��������.
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.f, 0.f);
    
    return Out;
}

PS_OUT PS_MAIN_RIMLIGHT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
    
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
    // �ؽ�ó�� �븻���� -1~1�� ����� ���ϱ⶧���� 0~1�� ����ȭ�Ǿ� �ִ�. ���� ���������� ��ȯ�������.
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);

    vNormal = mul(vNormal, WorldMatrix);
    
     // RimLight
    vector vCamDir = normalize(In.vWorldPos - g_vCamPos);
    float fRimLight = smoothstep(1.f - g_fRimWidth, 1.f, pow(1.f - max(0.f, dot(vNormal, vCamDir.xyz)), 2.f));

    Out.vDiffuse = vDiffuse;
    Out.vDiffuse.r += fRimLight;
    // UNORM 4�� Ÿ�Կ� ���� �����Ƿ� ���⼭ 0~1�� ����ó���ϰ� ���߿� ������ -1~1�� ����ó���� �ٽ��Ѵ�.
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    // SV_POSITION���� �������� �ʾҴ� ���������� ���̹Ƿ� w�����⸦ ������ z�� (���������̽�) ���� r, 
    // �ٽ� ���� ���̴����� w�� �����ֱ� ���� b�� ���� �ٽ� ��������.
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.f, 0.f);
    
    return Out;
}

PS_OUT PS_MAIN_DISSOLVE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
    vector maskColor = g_DissolveTexture.Sample(LinearSampler, In.vTexUV);
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexUV);
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f; // 0 ~ 1 -> -1 ~ 1
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
    float fProgress = saturate(1.f - g_fDissolveTimeAcc);
    float fDissolveFactor = (maskColor.r - fProgress) / g_fThickness;
    
    if (maskColor.r > fProgress + g_fThickness)
    {
        discard;
    }
    else if (maskColor.r > fProgress)
    {
        vDiffuse = lerp(vDiffuse, float4(1.0, 0.1, 0.1, 1.0), fDissolveFactor);
    }
    
    vNormal = mul(vNormal, WorldMatrix);
    
    Out.vDiffuse = vDiffuse;
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fCamFar, 0.f, 0.f);

    return Out;
}
technique11 DefaultTechnique
{
	pass Mesh
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

    pass MeshEmissive
	{
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader	= compile vs_5_0 VS_MAIN();
		GeometryShader	= NULL /*compile gs_5_0 GS_MAIN()*/;
		HullShader		= NULL /*compile hs_5_0 HS_MAIN()*/;
		DomainShader	= NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_EMISSIVE();
    }

    pass RimLight
	{
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader	= compile vs_5_0 VS_MAIN();
		GeometryShader	= NULL /*compile gs_5_0 GS_MAIN()*/;
		HullShader		= NULL /*compile hs_5_0 HS_MAIN()*/;
		DomainShader	= NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_RIMLIGHT();
    }

    pass Dissolve
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_DISSOLVE();
    }
}