#include "Shader_Client_Defines.hpp"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

vector			g_vCamPosition;
texture2D		g_Texture;



struct VS_IN
{
	/* 그리기 위한 정점정보 */
	float3		vPosition : POSITION;
	float2		vPSize : PSIZE;

	/* 인스턴싱정보 (도형하나를 제어하기위한 행렬) */
	float4		vRight : TEXCOORD1;
	float4		vUp : TEXCOORD2;
	float4		vLook : TEXCOORD3;
	float4		vTranslation : TEXCOORD4;
};


struct VS_OUT
{
	float4		vPosition : POSITION;
	float2		vPSize : PSIZE;
};


/* 정점을 받고 변환하고 정점을 리턴한다. */
VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	matrix			TransformMatrix;
	TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);

	vector			vPosition;
	vPosition = mul(vector(In.vPosition, 1.f), TransformMatrix);

	Out.vPosition = mul(vPosition, g_WorldMatrix);
	Out.vPSize = float2(In.vPSize.x * length(In.vRight), In.vPSize.y * length(In.vUp));
	
	return Out;
}

struct GS_IN
{
	float4		vPosition : POSITION;
	float2		vPSize : PSIZE;
};

struct GS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

[maxvertexcount(6)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> Triangles)
{
	GS_OUT		Out[4];

	vector		vLook = g_vCamPosition - In[0].vPosition;
	float3		vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook.xyz)) * In[0].vPSize.x * 0.5f;
	float3		vUp = normalize(cross(vLook.xyz, vRight)) * In[0].vPSize.y * 0.5f;

	matrix		matVP = mul(g_ViewMatrix, g_ProjMatrix);

	Out[0].vPosition = mul(vector(In[0].vPosition.xyz + vRight + vUp, 1.f), matVP);
	Out[0].vTexUV = float2(0.f, 0.f);

	Out[1].vPosition = mul(vector(In[0].vPosition.xyz - vRight + vUp, 1.f), matVP);
	Out[1].vTexUV = float2(1.f, 0.f);

	Out[2].vPosition = mul(vector(In[0].vPosition.xyz - vRight - vUp, 1.f), matVP);
	Out[2].vTexUV = float2(1.f, 1.f);

	Out[3].vPosition = mul(vector(In[0].vPosition.xyz + vRight - vUp, 1.f), matVP);
	Out[3].vTexUV = float2(0.f, 1.f);

	Triangles.Append(Out[0]);
	Triangles.Append(Out[1]);
	Triangles.Append(Out[2]);
	Triangles.RestartStrip();

	Triangles.Append(Out[0]);
	Triangles.Append(Out[2]);
	Triangles.Append(Out[3]);
	Triangles.RestartStrip();
	
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

	vColor = g_Texture.Sample(LinearSampler, In.vTexUV);

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
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL/*compile hs_5_0 HS_MAIN()*/;
		DomainShader = NULL/*compile ds_5_0 DS_MAIN()*/;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}