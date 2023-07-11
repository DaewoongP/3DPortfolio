#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CCollider;
END

BEGIN(Client)

class CBullet : public CGameObject
{
private:
	explicit CBullet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CBullet(const CBullet& rhs);
	virtual ~CBullet() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double dTimeDelta) override;
	virtual GAMEEVENT Late_Tick(_double dTimeDelta) override;
	virtual void OnCollisionEnter(COLLISIONDESC CollisionDesc) override;
	virtual HRESULT Render() override;
	virtual HRESULT Reset() override;

private:
	CShader*		m_pShaderCom = { nullptr };
	CRenderer*		m_pRendererCom = { nullptr };
	CCollider*		m_pColliderCom = { nullptr };

private:
	_double			m_dDeleteTimeAcc = { 0.0 };
	_double			m_dDeleteTime = { 5.0 };

public:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();
	void Fire(_vector vInitPosition, _vector vTargetPosition);

public:
	static CBullet* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END