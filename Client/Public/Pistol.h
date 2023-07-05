#pragma once
#include "Part.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
class CShader;
class CRenderer;
END

BEGIN(Client)

class CPistol final : public CPart
{
private:
	explicit CPistol(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CPistol(const CPistol& rhs);
	virtual ~CPistol() = default;

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

private:
	unordered_map<const _tchar*, class CGameObject*> m_Bullets;
	_int							m_iBulletIndex = { 0 };

public:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();
	void Fire(_vector vFirePosition, _vector vDirection);

public:
	static CPistol* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END