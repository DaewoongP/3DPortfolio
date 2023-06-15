#pragma once
#include "Dummy.h"

BEGIN(Engine)
class CMesh;
class CModel;
class CShader;
class CTexture;
class CRenderer;
class CCollider;
END

BEGIN(Tool)

class CAnimModel : public CDummy
{
private:
	explicit CAnimModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CAnimModel(const CAnimModel& rhs);
	virtual ~CAnimModel() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_double dTimeDelta) override;
	virtual void Late_Tick(_double dTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CModel*					m_pModelCom = { nullptr };
	CShader*				m_pShaderCom = { nullptr };
	CTexture*				m_pTextureCom = { nullptr };
	CRenderer*				m_pRendererCom = { nullptr };
	CCollider*				m_pColliderCom = { nullptr };

private:
	HRESULT	Add_Component(OBJECTDESC ObjectDesc);
	HRESULT	SetUp_ShaderResources();

public:
	static CAnimModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END