#include "Shader_Client_Defines.hpp"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D		g_Texture;


struct VS_IN
{
	/* 그리기 위한 정점정보 */
	float3		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;

	/* 인스턴싱정보 (도형하나를 제어하기위한 행렬) */
	float4		vRight : TEXCOORD1;
	float4		vUp : TEXCOORD2;
	float4		vLook : TEXCOORD3;
	float4		vTranslation : TEXCOORD4;
};


struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};


/* 정점을 받고 변환하고 정점을 리턴한다. */
VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	matrix			TransformMatrix;
	TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);

	vector			vPosition;
	vPosition = mul(vector(In.vPosition, 1.f), TransformMatrix);

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vPosition, matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

/* 픽셀을 받고 픽셀의 색을 결정하여 리턴한다. */
float4	PS_MAIN(PS_IN In) : SV_TARGET0
{
	float4		vColor = (float4)0;

	vColor = g_Texture.Sample(PointSampler, In.vTexUV);

    if (vColor.a < 0.1f)
        discard;

    vColor.rgb = float3(1.f, 0.f, 0.f);

    return vColor;
}


technique11		DefaultTechnique
{
	pass Default
	{
		SetRasterizerState(RS_Default);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL/*compile gs_5_0 GS_MAIN()*/;
		HullShader = NULL/*compile hs_5_0 HS_MAIN()*/;
		DomainShader = NULL/*compile ds_5_0 DS_MAIN()*/;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}