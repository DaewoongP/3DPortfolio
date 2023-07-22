#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
class CShader;
class CRenderer;
END

BEGIN(Client)

class CProp : public CGameObject
{
public:
	typedef struct tagPropDesc
	{
		_tchar pModelPrototypeTag[MAX_STR] = TEXT("");
		_float3 vScale = _float3(0.f, 0.f, 0.f);
		_float3 vRotation = _float3(0.f, 0.f, 0.f);
		_float4 vPosition = _float4(0.f, 0.f, 0.f, 1.f);
	}PROPDESC;

protected:
	explicit CProp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CProp(const CProp& rhs);
	virtual ~CProp() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Initialize_Level(_uint iLevelIndex) override;
	virtual void Tick(_double TimeDelta) override;
	virtual GAMEEVENT Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

protected:
	CModel*				m_pModelCom = { nullptr };
	CShader*			m_pShaderCom = { nullptr };
	CRenderer*			m_pRendererCom = { nullptr };

protected:
	PROPDESC			m_PropDesc;

private:
	HRESULT Add_Components();
	HRESULT Add_Components_Level(_uint iLevelIndex);
	HRESULT SetUp_ShaderResources();

public:
	static CProp* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END