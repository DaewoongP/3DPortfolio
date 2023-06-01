#pragma once
#include "GameObject.h"
#include "Tool_Defines.h"
// Ŭ���̾�Ʈ�� ������Ʈ���� ���������� �ʿ��� ������Ʈ�� �����Ѿ���.
//���� �������� �ʿ��Ұ�� ����, ����Ʈ �������� ��� push�ϴ� ���°� ������.
BEGIN(Engine)
class CRenderer;
class CTransform;
class CMesh;
class CShader;
class CCollider;
class CTexture;
END

BEGIN(Tool)

class CDummyObject : public CGameObject
{
private:
	explicit CDummyObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CDummyObject(const CDummyObject& rhs);
	virtual ~CDummyObject() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_double dTimeDelta) override;
	virtual void Late_Tick(_double dTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CRenderer*		m_pRendererCom = { nullptr };
	CTransform*		m_pTransform = { nullptr };
	CMesh*			m_pMeshCom = { nullptr };
	CShader*		m_pShaderCom = { nullptr };
	CCollider*		m_pColliderCom = { nullptr };
	CTexture*		m_pTextureCom = { nullptr };

private:
	HRESULT	Add_Component();
	HRESULT	SetUp_ShaderResources();

public:
	static CDummyObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pAtg) override;
	virtual void Free() override;
};

END