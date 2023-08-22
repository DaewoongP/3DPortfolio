
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D g_PostProcessingTexture;
texture2D g_BloomTexture;
texture2D g_BlurTexture;

bool g_isGrayScale, g_isRedScale, g_isBlur;

float BlurWeights[19] =
{
    0.0011, 0.0123, 0.0561, 0.1353, 0.278, 0.4868, 0.7261, 0.8712, 0.9231,
    0.9986, 0.9231, 0.8712, 0.7261, 0.4868, 0.278, 0.1353, 0.0561, 0.0123, 0.0011
};
float g_fTotal = 6.2108;
float g_iWinSizeX = 1280.f;
float g_iWinSizeY = 720.f;

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

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vTexUV = In.vTexUV;

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexUV : TEXCOORD0;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vOriginColor = g_PostProcessingTexture.Sample(LinearSampler, In.vTexUV);
    vector vBlurColor = g_BlurTexture.Sample(LinearSampler, In.vTexUV);
    
    if (true == g_isBlur)
        Out.vColor = vOriginColor + vBlurColor;
    else
        Out.vColor = vOriginColor;

    if (true == g_isGrayScale)
    {
        float fGrayScale = (Out.vColor.x + Out.vColor.y + Out.vColor.z) / 3.f;
        Out.vColor = fGrayScale;
    }
    
    if (true == g_isRedScale)
    {
        float fRedScale = (Out.vColor.x + Out.vColor.y + Out.vColor.z) / 3.f;
        Out.vColor.yz = fRedScale / 2.f;
        Out.vColor.x = fRedScale;
    }
        
    return Out;
}

struct PS_OUT_BLUR
{
    float4 vColor : SV_TARGET0;
};

PS_OUT_BLUR PS_MAIN_BLUR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vBloomOriginColor = g_BloomTexture.Sample(LinearSampler, In.vTexUV);
    vector vOriginColor = g_PostProcessingTexture.Sample(LinearSampler, In.vTexUV);
    
    vector vBlurColor = 0;

    float2 vUV = (float2) 0;
    
    float fTexU = 1.f / g_iWinSizeX;
    float fTexV = 1.f / g_iWinSizeY;
    
    int iBlurWeights = 9;
    
    for (int i = -iBlurWeights; i < iBlurWeights; ++i)
    {
        vUV = In.vTexUV + float2(fTexU * i, fTexV * i);
        vBlurColor += BlurWeights[iBlurWeights + i] * g_BloomTexture.Sample(LinearSampler, vUV);
    }
    
    vBlurColor /= g_fTotal;
    
    vector vBloom = pow(pow(abs(vBlurColor), 2.2f) + pow(abs(vBloomOriginColor), 2.2f), 1.f / 2.2f);

    Out.vColor = pow(abs(vOriginColor), 2.2f);
    vBloom = pow(abs(vBloom), 2.2f);
    
    Out.vColor += vBloom;
    
    Out.vColor = pow(abs(Out.vColor), 1.f / 2.2f);
 
    return Out;
}

struct PS_OUT_BLOOM
{
    float4 vColor : SV_TARGET0;
};

PS_OUT_BLOOM PS_MAIN_BLOOM(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_PostProcessingTexture.Sample(LinearSampler, In.vTexUV);

    //float3 vRGBWeight = float3(0.9126f, 0.7152f, 0.0722f);
    
    //float brightness = dot(Out.vColor.rgb, vRGBWeight);
    
    //if (0.5f > brightness)
    //    discard;
    
    Out.vColor = float4(Out.vColor.rgb, 1.f);
    
    return Out;
}

RasterizerState RS_Default
{
    FillMode = Solid;
    CullMode = Back;
    FrontCounterClockwise = false;
};

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

BlendState BS_Default
{
    BlendEnable[0] = false;
};

BlendState BS_BlendOne
{
// 렌더타겟 두개를 합칠 것이므로 0번, 1번 둘다 처리해줘야한다.
    BlendEnable[0] = true;
    BlendEnable[1] = true;

    SrcBlend = one;
    DestBlend = one;
    BlendOp = Add;
};

technique11 DefaultTechnique
{
    pass PostProcessing
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Depth_Disable, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass Blur
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Depth_Disable, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_BLUR();
    }

    pass Bloom
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Depth_Disable, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_BLOOM();
    }
}