#pragma once
#include "Part.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
class CShader;
class CRenderer;
class CCollider;
END

BEGIN(Client)

class CShuriken final : public CPart
{
private:
	explicit CShuriken(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CShuriken(const CShuriken& rhs);
	virtual ~CShuriken() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Initialize_ParentMatrix(PARENTMATRIXDESC ParentDesc) override;
	virtual void Tick(_double dTimeDelta) override;
	virtual GAMEEVENT Late_Tick(_double dTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CModel*			m_pModelCom = { nullptr };
	CShader*		m_pShaderCom = { nullptr };
	CRenderer*		m_pRendererCom = { nullptr };
	CCollider*		m_pColliderCom = { nullptr };

private:
	_bool			m_isAttacked = { false };
	_float4x4		m_OriginWorldMatrix;
	_float3			m_vAttackDir;

	_double			m_dTimeAcc = { 0.0 };

public:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();
	void Attack(_fvector vDirection);
	void AttackEnd();

public:
	static CShuriken* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END