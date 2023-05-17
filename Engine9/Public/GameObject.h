#pragma once
#include "Base.h"

BEGIN(Engine9)

class ENGINE_DLL CGameObject abstract : public CBase
{
protected:
	explicit CGameObject(LPDIRECT3DDEVICE9 pDevice);
	explicit CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_double TimeDelta);
	virtual void Late_Tick(_double TimeDelta);
	virtual HRESULT Render();

protected:
	LPDIRECT3DDEVICE9		 m_pDevice = { nullptr };

protected:
	unordered_map<const _tchar*, class CComponent*>		m_Components;

protected:
	HRESULT Add_Component(const _tchar * pPrototypeTag, const _tchar * pComponentTag, _Inout_ CComponent** ppOut, void* pArg = nullptr);

public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual void Free() override;
};

END