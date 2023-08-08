
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;
matrix g_LightViewMatrix, g_LightProjMatrix;

texture2D g_Texture;
vector g_vCamPosition;
float g_fCamFar;

texture2D g_NormalTexture;
texture2D g_DiffuseTexture;
texture2D g_ShadeTexture;
texture2D g_DepthTexture;
texture2D g_SpecularTexture;
texture2D g_EmissiveTexture;
texture2D g_LightDepthTexture;

vector g_vLightDir;
vector g_vLightPos;
float g_fLightRange;
float g_fSpotPower;
bool g_isGrayScale;
bool g_isRedScale;

vector g_vLightDiffuse;
vector g_vLightAmbient;
vector g_vLightSpecular;

vector g_vMtrlAmbient = vector(0.5f, 0.5f, 0.5f, 1.f);
vector g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);

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

matrix MyMatrixLookAtLH(float4 vEye, float4 vAt)
{
    matrix ViewMatrix = matrix(
    1.f, 0.f, 0.f, 0.f, 
    0.f, 1.f, 0.f, 0.f, 
    0.f, 0.f, 1.f, 0.f, 
    0.f, 0.f, 0.f, 1.f);
    
    vector vLook = float4(normalize(vAt.xyz - vEye.xyz), 0.f);
    vector vRight = float4(normalize(cross(float3(0.f, 1.f, 0.f), vLook.xyz)), 0.f);
    vector vUp = float4(normalize(cross(vLook.xyz, vRight.xyz)), 0.f);
    
    ViewMatrix = matrix(vRight, vUp, vLook, float4(0.f, 0.f, 0.f, 1.f));
    matrix TransposeViewMatrix = transpose(ViewMatrix);
    
    vector vPosition = float4(
    -1.f * dot(vEye, vRight),
    -1.f * dot(vEye, vUp),
    -1.f * dot(vEye, vLook),
    1.f);
    
    TransposeViewMatrix._41 = vPosition.x;
    TransposeViewMatrix._42 = vPosition.y;
    TransposeViewMatrix._43 = vPosition.z;
    TransposeViewMatrix._44 = vPosition.w;

    return TransposeViewMatrix;
}

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

/* 픽셀을 받고 픽셀의 색을 결정하여 리턴한다. */
PS_OUT PS_MAIN_DEBUG(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexUV);

    return Out;
}

struct PS_OUT_LIGHT
{
    float4 vShade : SV_TARGET0;
    float4 vSpecular : SV_TARGET1;
};

PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;

    vector vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexUV);

    vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

    Out.vShade = g_vLightDiffuse * saturate(max(dot(normalize(g_vLightDir) * -1.f, vNormal), 0.f) + (g_vLightAmbient * g_vMtrlAmbient));

    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexUV);
    float fViewZ = vDepthDesc.y * g_fCamFar;
    vector vPosition;

	/* 투영스페이스 상의 위치 */
    vPosition.x = In.vTexUV.x * 2.f - 1.f;
    vPosition.y = In.vTexUV.y * -2.f + 1.f;
    vPosition.z = vDepthDesc.x;
    vPosition.w = 1.f;

	/* 뷰스페이스 상의 위치. */
    vPosition = vPosition * fViewZ;
    vPosition = mul(vPosition, g_ProjMatrixInv);

	/* 월드스페이스 상의 위치. */
    vPosition = mul(vPosition, g_ViewMatrixInv);

    vector vReflect = reflect(normalize(g_vLightDir), vNormal);
    vector vLook = vPosition - g_vCamPosition;

    Out.vSpecular = (g_vLightSpecular) * (g_vMtrlSpecular) * pow(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f), 10.f);

    return Out;
}

PS_OUT_LIGHT PS_MAIN_POINT(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
    
    vector vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexUV);
    vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexUV);
    float fViewZ = vDepthDesc.y * g_fCamFar;
    
    vector vPosition;

	/* 투영스페이스 상의 위치 */
    vPosition.x = In.vTexUV.x * 2.f - 1.f;
    vPosition.y = In.vTexUV.y * -2.f + 1.f;
    vPosition.z = vDepthDesc.x;
    vPosition.w = 1.f;

	/* 뷰스페이스 상의 위치. */
    vPosition = vPosition * fViewZ;
    vPosition = mul(vPosition, g_ProjMatrixInv);

	/* 월드스페이스 상의 위치. */
    vPosition = mul(vPosition, g_ViewMatrixInv);

    vector vLightDir = vPosition - g_vLightPos;

    float fDistance = length(vLightDir);

    // 빛의 감쇄량을 0~1로 정규화 시켜 처리하기 위함.
    float fAtt = saturate((g_fLightRange - fDistance) / g_fLightRange);
	
    Out.vShade = g_vLightDiffuse * saturate(max(dot(normalize(vLightDir) * -1.f, vNormal), 0.f) + (g_vLightAmbient * g_vMtrlAmbient)) * fAtt;

    vector vReflect = reflect(normalize(vLightDir), vNormal);
    vector vLook = vPosition - g_vCamPosition;

    Out.vSpecular = (g_vLightSpecular) * (g_vMtrlSpecular) * pow(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f), 10.f) * fAtt;

    return Out;
}

PS_OUT_LIGHT PS_MAIN_SPOTLIGHT(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
    
    vector vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexUV);
    vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexUV);
    float fViewZ = vDepthDesc.y * g_fCamFar;
    
    vector vPosition;

	/* 투영스페이스 상의 위치 */
    vPosition.x = In.vTexUV.x * 2.f - 1.f;
    vPosition.y = In.vTexUV.y * -2.f + 1.f;
    vPosition.z = vDepthDesc.x;
    vPosition.w = 1.f;

	/* 뷰스페이스 상의 위치. */
    vPosition = vPosition * fViewZ;
    vPosition = mul(vPosition, g_ProjMatrixInv);

	/* 월드스페이스 상의 위치. */
    vPosition = mul(vPosition, g_ViewMatrixInv);

    vector vLightDir = vPosition - g_vLightPos;

    float fDistance = length(vLightDir);

    // spotlight factor
    float fSpot = pow(max(dot(normalize(vLightDir), g_vLightDir), 0.0f), g_fSpotPower);

	// 감쇠 효과
    // 실제 거리에 따른 공식처리 -> fSpot / fDistance * fDistance // -> f
    float fAtt = fSpot / fDistance * fDistance;
	
    Out.vShade = g_vLightDiffuse * saturate(max(dot(normalize(vLightDir) * -1.f, vNormal), 0.f) + (g_vLightAmbient * g_vMtrlAmbient)) * fAtt;

    vector vReflect = reflect(normalize(vLightDir), vNormal);
    vector vLook = vPosition - g_vCamPosition;

    Out.vSpecular = (g_vLightSpecular) * (g_vMtrlSpecular) * pow(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f), 10.f) * fAtt;

    return Out;
}

PS_OUT PS_MAIN_DEFERRED(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexUV);
    if (vDiffuse.a == 0.f)
        discard;
    
    float4 vFogColor = vector(0.1f, 0.1f, 0.1f, 0.1f);
    
    float fFogPower = 0.f;
    
    vector vDepthDesc = g_DepthTexture.Sample(LinearSampler, In.vTexUV);
    float fViewZ = vDepthDesc.y * g_fCamFar;
    vector vPosition;

	/* 투영스페이스 상의 위치 */
    vPosition.x = In.vTexUV.x * 2.f - 1.f;
    vPosition.y = In.vTexUV.y * -2.f + 1.f;
    vPosition.z = vDepthDesc.x;
    vPosition.w = 1.f;

	/* 뷰스페이스 상의 위치. */
    vPosition = vPosition * fViewZ;
    vPosition = mul(vPosition, g_ProjMatrixInv);

	/* 월드스페이스 상의 위치. */
    vPosition = mul(vPosition, g_ViewMatrixInv);

    if (vPosition.y >= 0.f)
        fFogPower = 0.f;
    else
        fFogPower = saturate(vPosition.y / -10.f);
    
    vector vShade = g_ShadeTexture.Sample(LinearSampler, In.vTexUV);
    vector vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexUV);
    vector vEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexUV);
    //vSpecular = (vector) 0;
    vSpecular *= 0.5f;
    
    Out.vColor = fFogPower * vFogColor + (1.f - fFogPower) * (vDiffuse * vShade + vSpecular + vEmissive);

    // 빛기준 포지션 정렬
    float4 vEye = vPosition + float4(-20.f, 20.f, -20.f, 1.f);
    matrix LightViewMatrix = MyMatrixLookAtLH(vEye, vPosition);
    
    vPosition = mul(vPosition, LightViewMatrix);

    vector vUVPos = mul(vPosition, g_LightProjMatrix);
    float2 vDepthTexUV;
	
    vDepthTexUV.x = (vUVPos.x / vUVPos.w) * 0.5f + 0.5f;
    vDepthTexUV.y = (vUVPos.y / vUVPos.w) * -0.5f + 0.5f;
    vector vLightDepth = g_LightDepthTexture.Sample(LinearSampler, vDepthTexUV);
    
    // 투영행렬을 곱해줬기때문에 w나누기 한 후 검사를 진행 해줘야한다.
    float3 vProjTest = vUVPos.xyz / vUVPos.w;
    if (-1.f > vProjTest.x ||
		1.f < vProjTest.x ||
		-1.f > vProjTest.y ||
		1.f < vProjTest.y||
        0.f > vProjTest.z ||
		1.f < vProjTest.z)
        Out.vColor = Out.vColor;
    // 투영행렬의 far를 다시곱해주어 포지션과 연산
    // 현재 픽셀의 깊이값과 해당하는 픽셀이 존재하는 빛기준의 텍스처 UV좌표 깊이값과 비교하여 처리한다.
    else if (vPosition.z - 0.1f > vLightDepth.r * g_fCamFar)
        Out.vColor = vector(0.f, 0.f, 0.f, 1.f);
    
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
    pass Debug
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_DEBUG();
    }

    pass Light_Directional
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Depth_Disable, 0);
        // 빛을 모두 더해서 적용하기 위한 BS
        SetBlendState(BS_BlendOne, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
    }

    pass Light_Point
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Depth_Disable, 0);
        // 점광원은 여러개로 처리될 수 있기 때문에 알파블렌드를 활용하여 픽셀의 색상값을 계속 더해주어
        // 하나의 렌더타겟에 모든값이 저장될 수 있도록 다중 광원을 처리해야한다.
        // 따라서 가지고있는 픽셀의 색상값을 그대로 가져와서 ADD 처리 해야함.
        SetBlendState(BS_BlendOne, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_POINT();
    }

    pass Light_Spotlight
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Depth_Disable, 0);
        // 점광원은 여러개로 처리될 수 있기 때문에 알파블렌드를 활용하여 픽셀의 색상값을 계속 더해주어
        // 하나의 렌더타겟에 모든값이 저장될 수 있도록 다중 광원을 처리해야한다.
        // 따라서 가지고있는 픽셀의 색상값을 그대로 가져와서 ADD 처리 해야함.
        SetBlendState(BS_BlendOne, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_SPOTLIGHT();
    }

    pass Deferred
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Depth_Disable, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL /*compile gs_5_0 GS_MAIN()*/;
        HullShader = NULL /*compile hs_5_0 HS_MAIN()*/;
        DomainShader = NULL /*compile ds_5_0 DS_MAIN()*/;
        PixelShader = compile ps_5_0 PS_MAIN_DEFERRED();
    }
}