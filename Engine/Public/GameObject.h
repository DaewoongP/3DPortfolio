#pragma once
#include "Composite.h"
#include "Transform.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CComposite
{
protected:
	explicit CGameObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	explicit CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

public:
	const _tchar*	Get_Tag() const { return m_pTag; }
	void			Set_Tag(const _tchar * pTag) { lstrcpy(m_pTag, pTag); }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg, CTransform::TRANSFORMDESC* pTransformDesc);
	virtual void	Tick(_double dTimeDelta) override;
	virtual GAMEEVENT Late_Tick(_double dTimeDelta) override;
	virtual void OnCollisionEnter(COLLISIONDESC CollisionDesc) {}
	virtual void OnCollisionStay(COLLISIONDESC CollisionDesc) {}
	virtual void OnCollisionExit(COLLISIONDESC CollisionDesc) {}
	virtual HRESULT Render() override;
	virtual HRESULT Reset() override;

protected:
	_tchar		m_pTag[MAX_STR] = TEXT("");

protected:
	class CTransform*			m_pTransformCom = { nullptr };

public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual void Free() override;
};

END