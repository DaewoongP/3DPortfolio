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

class CBoss_Sword final : public CPart
{
private:
	explicit CBoss_Sword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CBoss_Sword(const CBoss_Sword& rhs);
	virtual ~CBoss_Sword() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Initialize_ParentMatrix(PARENTMATRIXDESC ParentDesc) override;
	virtual void Tick(_double dTimeDelta) override;
	virtual GAMEEVENT Late_Tick(_double dTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CModel*				m_pModelCom = { nullptr };
	CShader*			m_pShaderCom = { nullptr };
	CRenderer*			m_pRendererCom = { nullptr };
	CCollider*			m_pColliderCom = { nullptr };

public:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();
	void Attack();
	void AttackEnd();
	void Blocked();

public:
	static CBoss_Sword* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END