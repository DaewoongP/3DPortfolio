#pragma once

#include "Composite.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CComposite
{
protected:
	explicit CGameObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	explicit CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

public:
	void Set_Tag(const _tchar * pTag) {
		lstrcpy(m_pTag, pTag);
	}
	const _tchar* Get_Tag() const { return m_pTag; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_double dTimeDelta);
	virtual void Late_Tick(_double dTimeDelta);
	virtual HRESULT Render();

protected:
	_tchar m_pTag[MAX_STR];

public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual void Free() override;
};

END