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
	void Set_Owner(class CComposite* pOwner) { m_pOwner = pOwner; }
	void Set_PrototypeTag(const _tchar * pPrototypeTag) {
		lstrcpy(m_pPrototypeTag, pPrototypeTag);
	}
	class CComposite* Get_Owner() { return m_pOwner; }
	const _tchar* Get_PrototypeTag() const { return m_pPrototypeTag; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual GAMEEVENT Tick(_double dTimeDelta);
	virtual void Late_Tick(_double dTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Reset();

protected:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };

protected:
	class CComposite*			m_pOwner = { nullptr };
	_bool						m_isCloned = { false };

protected:
	_tchar			m_pPrototypeTag[MAX_STR] = TEXT("");

public:
	virtual CComponent* Clone(void* pArg) PURE;
	virtual void Free() override;
};

END