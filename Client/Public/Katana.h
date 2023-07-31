#pragma once
#include "Part.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
class CShader;
class CTexture;
class CRenderer;
class CCollider;
class CVIBuffer_Rect_Trail;
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
	virtual HRESULT Initialize_Level(_uint iLevelIndex) override;
	virtual HRESULT Initialize_ParentMatrix(PARENTMATRIXDESC ParentDesc) override;
	virtual void OnCollisionEnter(COLLISIONDESC CollisionDesc) override;
	virtual void Tick(_double dTimeDelta) override;
	virtual GAMEEVENT Late_Tick(_double dTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CModel*					m_pModelCom = { nullptr };
	CShader*				m_pShaderCom = { nullptr };
	CShader*				m_pTrailShaderCom = { nullptr };
	CTexture*				m_pTrailTextureCom = { nullptr };
	CRenderer*				m_pRendererCom = { nullptr };
	CCollider*				m_pColliderCom = { nullptr };
	CVIBuffer_Rect_Trail*	m_pTrailBufferCom = { nullptr };

private:
	_uint					m_iTrailFrontIndex = { 0 };
	_uint					m_iTrailBackIndex = { 0 };

private:
	HRESULT Add_Components();
	HRESULT Add_Components_Level(_uint iLevelIndex);
	HRESULT SetUp_ShaderResources();

public:
	void Attack();

public:
	static CKatana* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END