#pragma once
#include "Prop.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)

class CColProp : public CProp
{
public:
	typedef struct tagColPropDesc : public tagPropDesc
	{
		const _tchar*	szColliderFilePath = TEXT("");
	}COLPROPDESC;

private:
	explicit CColProp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CColProp(const CColProp& rhs);
	virtual ~CColProp() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual GAMEEVENT Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Add_Components();
	HRESULT SetUp_Collider(const _tchar * pColliderFilePath);

private:
	CCollider*		m_pColliderCom = { nullptr };

public:
	static CColProp* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END