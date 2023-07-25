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

class CBomb final : public CGameObject
{
private:
	explicit CBomb(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CBomb(const CBomb& rhs);
	virtual ~CBomb() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double dTimeDelta) override;
	virtual GAMEEVENT Late_Tick(_double dTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Reset() override;

private:
	CModel*			m_pModelCom = { nullptr };
	CShader*		m_pShaderCom = { nullptr };
	CRenderer*		m_pRendererCom = { nullptr };
	CCollider*		m_pColliderCom = { nullptr };

private:
	_double			m_dExplodeTimeAcc = { 0.0 };
	_double			m_dExplodeTime = { 3.0 };
	_double			m_dDeleteTime = { 3.5 };

public:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();
	void Fire(_fvector vInitPosition, _fvector vTargetPosition);

public:
	static CBomb* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END