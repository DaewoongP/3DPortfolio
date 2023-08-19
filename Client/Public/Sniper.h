#pragma once
#include "Part.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
class CShader;
class CRenderer;
END

BEGIN(Client)

class CSniper final : public CPart
{
private:
	explicit CSniper(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CSniper(const CSniper& rhs);
	virtual ~CSniper() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Initialize_ParentMatrix(PARENTMATRIXDESC ParentDesc) override;
	virtual void Tick(_double dTimeDelta) override;
	virtual GAMEEVENT Late_Tick(_double dTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CModel*					m_pModelCom = { nullptr };
	CShader*				m_pShaderCom = { nullptr };
	CRenderer*				m_pRendererCom = { nullptr };

private:
	multimap<const _tchar*, class CGameObject*>		m_Bullets;
	_int											m_iBulletIndex = { 0 };

public:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();
	void Attack(_vector vPosition, _vector vTargetPos, _double dSpeed);
	void Delete_Bullet();

private:
	void Bullet_Late_Tick(_double dTimeDelta);

public:
	static CSniper* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END