matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

sampler LinearSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = WRAP;
    AddressV = WRAP;
};

RasterizerState rasterizer
{
    cullmode = NONE;
    fillmode = WIREFRAME;
};

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexUV : TEXCOORD0;
};

struct VS_OUT_PHONG
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexUV : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
};

VS_OUT_PHONG VS_MAIN_PHONG(VS_IN In)
{
    VS_OUT_PHONG Out = (VS_OUT_PHONG) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    vector vWorldNormal = mul(vector(In.vNormal, 0.f), g_WorldMatrix);
    Out.vNormal = normalize(vWorldNormal);
    Out.vTexUV = In.vTexUV;
    Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);

    return Out;
}

struct PS_IN_PHONG
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexUV : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
};

float4 PS_MAIN_PHONG(PS_IN_PHONG In) : SV_TARGET0
{
    float4 vColor = (float4) 0;
    
    if (In.vWorldPos.x < 1.f && In.vWorldPos.z == 0.f)
        discard;
    if (In.vWorldPos.x == 0.f && In.vWorldPos.z < 1.f)
        discard;
    
    vColor = float4(0.f, 1.f, 1.f, 1.f);
    return vColor;
}

technique11 DefaultTechnique
{
    pass Terrain_Phong
    {
        SetRasterizerState(rasterizer);
        VertexShader = compile vs_5_0 VS_MAIN_PHONG();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_PHONG();
    }
}