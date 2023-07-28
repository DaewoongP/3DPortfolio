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

class CBoss_Shield final : public CPart
{
private:
	explicit CBoss_Shield(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CBoss_Shield(const CBoss_Shield& rhs);
	virtual ~CBoss_Shield() = default;

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
	void Blocked();

public:
	static CBoss_Shield* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END