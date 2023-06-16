#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CComponent abstract : public CBase
{
protected:
	explicit CComponent(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	explicit CComponent(const CComponent& rhs);
	virtual ~CComponent() = default;

public:
	void Set_Tag(const _tchar * pTag) {
		lstrcpy(m_pPrototypeTag, pTag);
	}
	const _tchar* Get_Tag() const { return m_pPrototypeTag; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_double dTimeDelta);
	virtual void Late_Tick(_double dTimeDelta);
	virtual HRESULT Render();

protected:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

protected:
	_bool			m_bIsClone = { false };

protected:
	_tchar			m_pPrototypeTag[MAX_STR];

public:
	virtual CComponent* Clone(void* pArg) PURE;
	virtual void Free() override;
};

END