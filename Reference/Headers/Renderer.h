#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CRenderer final : public CComponent
{
public:
	enum RENDERGROUP {RENDER_PRIORITY, RENDER_LIGHTDEPTH, RENDER_NONBLEND, RENDER_NONLIGHT, RENDER_BLEND, RENDER_SCREEN, RENDER_UI, RENDER_END };

private:
	explicit CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderer() = default;

public:
	void Set_Bloom(_bool isBloom) { m_isBloom = isBloom; }
	void Set_RedScale(_bool isRedScale) { m_isRedScale = isRedScale; }
	void Set_BlueScale(_bool isBlueScale, _float4 vBlueScaleLevel) { 
		m_isBlueScale = isBlueScale;
		m_vBlueScaleLevel = vBlueScaleLevel;
	}
	void Set_BlueScale(_bool isBlueScale) { m_isBlueScale = isBlueScale; }
	void Set_GrayScale(_bool isGrayScale) { m_isGrayScale = isGrayScale; }
	void Set_MotionBlur(_bool isMotionBlur) { m_isMotionBlur = isMotionBlur; }

public:
	virtual HRESULT Initialize_Prototype() override;

public:
	void	Add_RenderGroup(RENDERGROUP eRenderGroup, class CGameObject* pGameObject);
	HRESULT Draw_RenderGroup();

#ifdef _DEBUG
public:
	HRESULT Add_DebugGroup(CComponent* pDebugCom);
#endif // _DEBUG

private:
	HRESULT Render_Priority();
	HRESULT Render_LightDepth();
	HRESULT Render_NonBlend();
	HRESULT Render_Lights();
	HRESULT Render_SSAO();
	HRESULT Render_Deferred();
	HRESULT Render_Effect();
	HRESULT Render_NonLight();
	HRESULT Render_Blend();
	HRESULT Render_MotionBlur();
	HRESULT Render_DepthOfField();
	HRESULT Render_Bloom();
	HRESULT Render_Blur();
	HRESULT Render_PostProcessing();
	HRESULT Render_Screen();
	HRESULT Render_UI();

private:
	// ���ĺ��� ��ü�� �׸��� ���̿� ���� ���� ������ ���� �Լ�.
	// ī�޶� ��ġ�� �������� �հͺ��� �׷� ó����.
	HRESULT Sort_Blend();
	HRESULT Sort_UI();
	HRESULT Add_Components();
	HRESULT Create_DepthTexture(_uint iSizeX, _uint iSizeY);

#ifdef _DEBUG
private:
	HRESULT Render_Debug();
	HRESULT Render_DebugTarget();
#endif // _DEBUG

private:	
	list<class CGameObject*>		m_RenderObjects[RENDER_END];
#ifdef _DEBUG
private:
	list<class CComponent*>			m_DebugObject;
	_bool							m_isDebugRender = { true };
	_bool							m_isDebugRenderTarget = { true };
#endif // _DEBUG
	

private:
	_bool							m_isGrayScale = { false };
	_bool							m_isRedScale = { false };
	_bool							m_isBlueScale = { false };
	_float4							m_vBlueScaleLevel;
	_bool							m_isBloom = { false };
	_bool							m_isMotionBlur = { false };

private:
	class CRenderTarget_Manager*	m_pRenderTarget_Manager = { nullptr };
	class CLight_Manager*			m_pLight_Manager = { nullptr };

private: /* Deferred Shader */
	class CVIBuffer_Rect*			m_pDeferredBuffer = { nullptr };
	class CShader*					m_pDeferredShader = { nullptr };
	_float4x4						m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;

private: /* Post Processing */
	class CVIBuffer_Rect*			m_pPostProcessingBuffer = { nullptr };
	class CShader*					m_pPostProcessingShader = { nullptr };

private: /* Light Depth Target */
	_uint2							m_iShadowMapSize;
	ID3D11DepthStencilView*			m_pDSV = { nullptr };
	ID3D11Texture2D*				m_pShadowDepthTexture2D = { nullptr };

public:
	static CRenderer* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END