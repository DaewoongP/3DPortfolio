#pragma once
#include "GameObject.h"
#include "Tool_Defines.h"

BEGIN(Engine)
class CMesh;
class CModel;
class CShader;
class CTexture;
class CRenderer;
class CCollider;
class CTransform;
END

BEGIN(Tool)

class CNonAnimModel : public CGameObject
{
public:
	typedef struct tagDummyObjectDesc
	{
		_tchar pModelPrototypeTag[MAX_STR] = TEXT("");
		_float4 vPosition = _float4(0.f, 0.f, 0.f, 1.f);
	}OBJECTDESC;
private:
	explicit CNonAnimModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CNonAnimModel(const CNonAnimModel& rhs);
	virtual ~CNonAnimModel() = default;

public:
	CTransform* Get_TransformCom() const { return m_pTransformCom; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_double dTimeDelta) override;
	virtual void Late_Tick(_double dTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Move_Position(_fvector vPos, _double dTimeDelta);

private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };

private:
	HRESULT	Add_Component(OBJECTDESC ObjectDesc);
	HRESULT	SetUp_ShaderResources();

public:
	static CNonAnimModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pAtg) override;
	virtual void Free() override;
};

END