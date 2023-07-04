#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CRenderer;
class CNavigation;
END

BEGIN(Client)

class CEnemy abstract : public CGameObject
{
public:
	typedef struct tagEnemyDesc
	{
		_tchar pModelPrototypeTag[MAX_STR] = TEXT("");
		_float3 vScale = _float3(0.f, 0.f, 0.f);
		_float3 vRotation = _float3(0.f, 0.f, 0.f);
		_float4 vPosition = _float4(0.f, 0.f, 0.f, 1.f);
	}ENEMYDESC;

protected:
	explicit CEnemy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CEnemy(const CEnemy& rhs);
	virtual ~CEnemy() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double dTimeDelta) override;
	virtual GAMEEVENT Late_Tick(_double dTimeDelta) override;
	virtual void OnCollisionEnter(COLLISIONDESC CollisionDesc) override;
	virtual void OnCollisionStay(COLLISIONDESC CollisionDesc) override;
	virtual void OnCollisionExit(COLLISIONDESC CollisionDesc) override;
	virtual HRESULT Render() override;
	virtual HRESULT Reset() override;

protected:
	CRenderer*				m_pRendererCom = { nullptr };
	CNavigation*			m_pNavigationCom = { nullptr };

private: /* Tick */
	HRESULT Add_Component();

public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual void Free() override;
};

END