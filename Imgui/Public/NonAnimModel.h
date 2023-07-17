#pragma once
#include "Dummy.h"

BEGIN(Engine)
class CModel;
class CShader;
class CRenderer;
END

BEGIN(Tool)

class CNonAnimModel : public CDummy
{
private:
	explicit CNonAnimModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CNonAnimModel(const CNonAnimModel& rhs);
	virtual ~CNonAnimModel() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_double dTimeDelta) override;
	virtual GAMEEVENT Late_Tick(_double dTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };

private:
	HRESULT	Add_Component(OBJECTDESC ObjectDesc);
	HRESULT	SetUp_ShaderResources();

public:
	static CNonAnimModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END