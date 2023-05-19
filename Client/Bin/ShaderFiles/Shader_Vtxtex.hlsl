struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexUV : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
    float2 vTexUV : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
    
    
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_Position;
    float2 vTexUV : TEXCOORD0;
};

vector PS_MAIN(PS_IN In)
{
    float4 vColor = (float4) 0;

    return vColor;
}

technique11 DefaultTechnique
{
    pass BackGround
    {
        VertexShader    = compile vs_5_0 VS_MAIN();
        GeometryShader  = NULL; //compile gs_5_0 GS_Main();
        HullShader      = NULL; //compile hs_5_0 HS_Main();
        DomainShader    = NULL; //compile ds_5_0 DS_Main();
        PixelShader     = compile ps_5_0 PS_MAIN();
    }

    pass UI
    {
        VertexShader    = compile vs_5_0 VS_MAIN();
        GeometryShader  = NULL; //compile gs_5_0 GS_Main();
        HullShader      = NULL; //compile hs_5_0 HS_Main();
        DomainShader    = NULL; //compile ds_5_0 DS_Main();
        PixelShader     = compile ps_5_0 PS_MAIN();
    }
}