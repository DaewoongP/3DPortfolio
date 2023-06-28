#pragma once
#include "Part.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
class CShader;
class CRenderer;
class CTransform;
END

BEGIN(Client)

class CKatana final : public CPart
{
private:
	explicit CKatana(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CKatana(const CKatana& rhs);
	virtual ~CKatana() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double dTimeDelta) override;
	virtual void Late_Tick(_double dTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CModel*					m_pModelCom = { nullptr };
	CShader*				m_pShaderCom = { nullptr };
	CRenderer*				m_pRendererCom = { nullptr };
	CTransform*				m_pTransformCom = { nullptr };

private:
	_uint					m_iPlayerWeaponIndex = { 0 };
	CModel*					m_pPlayerModel = { nullptr };

public:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();
	HRESULT Find_BoneIndices();

public:
	static CKatana* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END