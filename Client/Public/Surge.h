#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
class CShader;
class CRenderer;
class CCollider;
END

BEGIN(Client)

class CSurge final : public CGameObject
{
private:
	explicit CSurge(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CSurge(const CSurge& rhs);
	virtual ~CSurge() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double dTimeDelta) override;
	virtual GAMEEVENT Late_Tick(_double dTimeDelta) override;
	virtual void OnCollisionEnter(COLLISIONDESC CollisionDesc) override;
	virtual HRESULT Render() override;
	virtual HRESULT Reset() override;

private:
	CModel*			m_pModelCom = { nullptr };
	CShader*		m_pShaderCom = { nullptr };
	CRenderer*		m_pRendererCom = { nullptr };
	CCollider*		m_pColliderCom = { nullptr };

private:
	_double			m_dDeleteTimeAcc = { 0.0 };
	_double			m_dDeleteTime = { 0.0 };

private:
	_float4			m_vEmissiveColor;
	_bool			m_isAttacked = { false };

public:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();
	void Fire(_fvector vInitPosition, _fvector vTargetPosition);

public:
	static CSurge* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END