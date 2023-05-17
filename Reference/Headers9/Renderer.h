#pragma once
#include "Component.h"

BEGIN(Engine9)

class ENGINE_DLL CRenderer final : public CComponent
{
public:
	enum class RENDERGROUP { RENDER_PRIORITY, RENDER_NONBLEND, RENDER_NONLIGHT, RENDER_BLEND, RENDER_UI, RENDER_END };

protected:
	explicit CRenderer(LPDIRECT3DDEVICE9 pDevice);
	explicit CRenderer(const CComponent& rhs);
	virtual ~CRenderer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	void Add_RenderGroup(RENDERGROUP eRenderGroup, class CGameObject* pGameObject);
	HRESULT Draw_RenderGroup();

private:
	HRESULT Render_Priority();
	HRESULT Render_NonBlend();
	HRESULT Render_NonLight();
	HRESULT Render_Blend();
	HRESULT Render_UI();

private:
	list<class CGameObject*>				m_RenderObjects[static_cast<_uint>(RENDERGROUP::RENDER_END)];

public:
	static CRenderer* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END