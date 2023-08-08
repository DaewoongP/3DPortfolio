#include "Shader_Client_Defines.hpp"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

vector			g_vCamPosition;
texture2D		g_Texture;

struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vPSize : PSIZE;
	
	float4		vRight : TEXCOORD1;
	float4		vUp : TEXCOORD2;
	float4		vLook : TEXCOORD3;
	float4		vTranslation : TEXCOORD4;
    float4		vColor : TEXCOORD5;
};

struct VS_OUT
{
	float4		vPosition : POSITION;
	float2		vPSize : PSIZE;
	float4		vColor : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	matrix			TransformMatrix;
	TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);

	vector			vPosition;
	vPosition = mul(vector(In.vPosition, 1.f), TransformMatrix);

	Out.vPosition = mul(vPosition, g_WorldMatrix);
	Out.vPSize = float2(In.vPSize.x * length(In.vRight), In.vPSize.y * length(In.vUp));
    Out.vColor = In.vColor;
	
	return Out;
}

struct GS_IN
{
	float4		vPosition : POSITION;
	float2		vPSize : PSIZE;
    float4		vColor : TEXCOORD0;
};

struct GS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
    float4		vColor : TEXCOORD1;
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
    Out[0].vColor = In[0].vColor;

	Out[1].vPosition = mul(vector(In[0].vPosition.xyz - vRight + vUp, 1.f), matVP);
	Out[1].vTexUV = float2(1.f, 0.f);
    Out[1].vColor = In[0].vColor;

	Out[2].vPosition = mul(vector(In[0].vPosition.xyz - vRight - vUp, 1.f), matVP);
	Out[2].vTexUV = float2(1.f, 1.f);
    Out[2].vColor = In[0].vColor;

	Out[3].vPosition = mul(vector(In[0].vPosition.xyz + vRight - vUp, 1.f), matVP);
	Out[3].vTexUV = float2(0.f, 1.f);
    Out[3].vColor = In[0].vColor;

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
    float4		vColor : TEXCOORD1;
};

float4	PS_MAIN(PS_IN In) : SV_TARGET0
{
    float4 vColor = (float4) 0;
    
    float4 vTexture = g_Texture.Sample(LinearSampler, In.vTexUV);
    
    vColor.a = (vTexture.r + vTexture.g + vTexture.b) / 3.f;
    vColor.rgb = In.vColor.rgb;
    
    return vColor;
}

float4 PS_MAIN_ALPHA(PS_IN In) : SV_TARGET0
{
    float4 vColor = (float4) 0;
    
    float4 vTexture = g_Texture.Sample(LinearSampler, In.vTexUV);
    
    vColor.a = (vTexture.r + vTexture.g + vTexture.b) / 3.f;
    vColor.rgb = In.vColor.rgb;
    vColor.a *= In.vColor.a;
	
    if (0.1f > vColor.a)
        discard;
	
	return vColor;
}

technique11		DefaultTechnique
{
	pass ColorInstance
	{
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		HullShader = NULL/*compile hs_5_0 HS_MAIN()*/;
		DomainShader = NULL/*compile ds_5_0 DS_MAIN()*/;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

    pass Alpha
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_ALPHA();
    }
}