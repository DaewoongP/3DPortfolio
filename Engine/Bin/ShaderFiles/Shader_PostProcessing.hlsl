
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D g_PostProcessingTexture;
texture2D g_EffectTexture;
texture2D g_BloomTexture;
texture2D g_BlurTexture;
texture2D g_DepthTexture;
texture2D g_DepthOfFieldTexture;
texture2D g_FinalTexture;

bool g_isGrayScale, g_isRedScale, g_isBlueScale, g_isBloom, g_isMotionBlur;
float g_fCamFar;
float4 g_vBlueScaleLevel;

// 가우시안 필터 1차원 배열형태
float BlurWeights[23] =
{
    0.0011, 0.0123, 0.0561, 0.1353, 0.278, 0.3001, 0.4868, 0.6666, 0.7261, 0.8712, 0.9231,
    0.9986, 0.9231, 0.8712, 0.7261, 0.6666, 0.4868, 0.3001, 0.278, 0.1353, 0.0561, 0.0123, 0.0011
};

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
    vector vDOFColor = g_DepthOfFieldTexture.Sample(LinearSampler, In.vTexUV);

    if (true == g_isBloom)
    {
        if (vDOFColor.x > 0.1f)
            Out.vColor = vDOFColor + vBlurColor;
        else
            Out.vColor = vOriginColor + vBlurColor;
    }
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
    
    if (true == g_isBlueScale)
    {
        Out.vColor *= g_vBlueScaleLevel;
    }
    
    return Out;
}

struct PS_OUT_BLOOMBLUR
{
    float4 vColor : SV_TARGET0;
};

PS_OUT_BLOOMBLUR PS_MAIN_BLOOMBLUR(PS_IN In)
{
    PS_OUT_BLOOMBLUR Out = (PS_OUT_BLOOMBLUR) 0;

    vector vBloomOriginColor = g_BloomTexture.Sample(LinearSampler, In.vTexUV);
    vector vOriginColor = g_EffectTexture.Sample(LinearSampler, In.vTexUV);
    
    vector vBlurColor = 0;

    float2 vUV = (float2) 0;
    
    float fTexU = 1.f / g_iWinSizeX;
    float fTexV = 1.f / g_iWinSizeY;
    
    int iBlurWeights = 11;
    float fTotal = 0.f;
    
    for (int i = -iBlurWeights; i < iBlurWeights; ++i)
    {
        vUV = In.vTexUV + float2(fTexU * i, fTexV * i);
        vBlurColor += BlurWeights[iBlurWeights + i] * g_BloomTexture.Sample(LinearSampler, vUV);
        fTotal += BlurWeights[iBlurWeights + i];
    }

    vBlurColor /= fTotal;
    
    // 커질수록 은은한 빛, 작아질수록 강한 빛
    float fBloomPower = 3.f;
    
    vector vBloom = pow(pow(abs(vBlurColor), fBloomPower) + pow(abs(vBloomOriginColor), fBloomPower), 1.f / fBloomPower);

    Out.vColor = pow(abs(vOriginColor), fBloomPower);
    vBloom = pow(abs(vBloom), fBloomPower);
    
    Out.vColor += vBloom;
    
    Out.vColor = pow(abs(Out.vColor), 1.f / fBloomPower);
 
    return Out;
}

struct PS_OUT_BLOOM
{
    float4 vColor : SV_TARGET0;
};

PS_OUT_BLOOM PS_MAIN_BLOOM(PS_IN In)
{
    PS_OUT_BLOOM Out = (PS_OUT_BLOOM) 0;

    Out.vColor = g_EffectTexture.Sample(LinearSampler, In.vTexUV);

    // 밝은 부분 추출
    //float3 vRGBWeight = float3(0.9126f, 0.7152f, 0.0722f);
    
    //float brightness = dot(Out.vColor.rgb, vRGBWeight);
    
    //if (0.99f > brightness)
    //    discard;
    
    Out.vColor = float4(Out.vColor.rgb, 1.f);
    
    return Out;
}

struct PS_OUT_DOF
{
    float4 vColor : SV_TARGET0;
};

PS_OUT_DOF PS_MAIN_DOF(PS_IN In)
{
    PS_OUT_DOF Out = (PS_OUT_DOF) 0;

    vector vBlurTexture = g_PostProcessingTexture.Sample(LinearSampler, In.vTexUV);
    vector vDepthDesc = g_DepthTexture.Sample(LinearSampler, In.vTexUV);

    float2 vUV = (float2) 0;
    
    float fTexU = 1.f / g_iWinSizeX;
    float fTexV = 1.f / g_iWinSizeY;
    
    int iBlurWeights;
    float fTotal = 0.f;
    
    float fViewZ = vDepthDesc.y * g_fCamFar;
    
    if (vDepthDesc.y > 0.2f)
    {
        iBlurWeights = 5;
        
        for (int i = -iBlurWeights; i < iBlurWeights; ++i)
        {
            vUV = In.vTexUV + float2(fTexU * i, fTexV * i);
            Out.vColor += BlurWeights[iBlurWeights + i] * g_PostProcessingTexture.Sample(LinearSampler, vUV);
            fTotal += BlurWeights[iBlurWeights + i];
        }
    }

    Out.vColor /= fTotal;

    return Out;
}

PS_OUT_DOF PS_MAIN_MBLUR(PS_IN In)
{
    PS_OUT_DOF Out = (PS_OUT_DOF) 0;
    
    float2 vTexUV = 0.f;
    
    float2 fTexelSize = float2(1.f / g_iWinSizeX, 1.f / g_iWinSizeY);
    float2 vFocus = In.vTexUV - float2(0.5f, 0.5f);
    
    float fFocusDetail = 7.f;
    float fFocusPower = 30.f;
    
    if (true == g_isMotionBlur)
    {
        for (float i = 0.f; i < fFocusDetail; i += 1.f)
        {
            float fScale = 1.f - fFocusPower * fTexelSize.x * i;
            vTexUV = vFocus * fScale + float2(0.5f, 0.5f);
            Out.vColor += g_FinalTexture.Sample(LinearSampler, vTexUV);
        }
    
        Out.vColor *= 1.f / fFocusDetail;
    }
    else
    {
        Out.vColor = g_FinalTexture.Sample(LinearSampler, In.vTexUV);
    }
    
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
        PixelShader = compile ps_5_0 PS_MAIN_BLOOMBLUR();
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
    
    pass DepthOfField
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Depth_Disable, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_DOF();
    }

    pass MotionBlur
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Depth_Disable, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_MBLUR();
    }
}