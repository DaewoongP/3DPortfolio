#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CRenderer final : public CComponent
{
public:
	enum RENDERGROUP { RENDER_PRIORITY, RENDER_NONBLEND, RENDER_NONLIGHT, RENDER_BLEND, RENDER_UI, RENDER_END };

protected:
	explicit CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Reset() override;

public:
	void	Add_RenderGroup(RENDERGROUP eRenderGroup, class CGameObject* pGameObject);
	HRESULT Draw_RenderGroup();

private:
	HRESULT Render_Priority();
	HRESULT Render_NonBlend();
	HRESULT Render_NonLight();
	HRESULT Render_Blend();
	HRESULT Render_UI();

#ifdef _DEBUG
private:
	HRESULT Render_Debug();
#endif // _DEBUG

private:	
	list<class CGameObject*>				m_RenderObjects[RENDER_END];

private:
	class CRenderTarget_Manager*			m_pRenderTarget_Manager = { nullptr };

private:
	class CVIBuffer_Rect*					m_pVIBuffer = { nullptr };
	class CShader*							m_pShader = { nullptr };
	_float4x4								m_ViewMatrix, m_ProjMatrix;

public:
	static CRenderer* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END