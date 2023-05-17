#pragma once

#include "Base.h"

BEGIN(Engine9)

class ENGINE_DLL CComponent abstract : public CBase
{
protected:
	explicit CComponent(LPDIRECT3DDEVICE9 pDevice);
	explicit CComponent(const CComponent& rhs);
	virtual ~CComponent() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

protected:
	LPDIRECT3DDEVICE9	 m_pDevice = { nullptr };

public:
	virtual CComponent* Clone(void* pArg) PURE;
	virtual void Free() override;
};

END