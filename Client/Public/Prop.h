#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
class CShader;
class CRenderer;
END

BEGIN(Client)

class CProp final : public CGameObject
{
public:
	typedef struct tagPropDesc
	{
		_tchar pModelPrototypeTag[MAX_STR] = TEXT("");
		_float3 vScale = _float3(0.f, 0.f, 0.f);
		_float3 vRotation = _float3(0.f, 0.f, 0.f);
		_float4 vPosition = _float4(0.f, 0.f, 0.f, 1.f);
	}PROPDESC;

private:
	explicit CProp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CProp(const CProp& rhs);
	virtual ~CProp() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };

public:
	HRESULT Add_Components(PROPDESC PropDesc);
	HRESULT SetUp_ShaderResources();

public:
	static CProp* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END