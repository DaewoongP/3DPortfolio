#pragma once
#include "GameObject.h"
#include "Tool_Defines.h"
// Ŭ���̾�Ʈ�� ������Ʈ���� ���������� �ʿ��� ������Ʈ�� �����Ѿ���.
//���� �������� �ʿ��Ұ�� ����, ����Ʈ �������� ��� push�ϴ� ���°� ������.
BEGIN(Engine)
class CMesh;
class CModel;
class CShader;
class CTexture;
class CRenderer;
class CCollider;
class CTransform;
class CVIBuffer_Cube;
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

public:
	void Move_Position(_fvector vPos, _double dTimeDelta);

private:
	CMesh*					m_pMeshCom = { nullptr };
	CModel*					m_pModelCom = { nullptr };
	CShader*				m_pShaderCom = { nullptr };
	CTexture*				m_pTextureCom = { nullptr };
	CRenderer*				m_pRendererCom = { nullptr };
	CCollider*				m_pColliderCom = { nullptr };
	CTransform*				m_pTransformCom = { nullptr };

private:
	CVIBuffer_Cube* m_pBufferCom = { nullptr };

private:
	HRESULT	Add_Component();
	HRESULT	SetUp_ShaderResources();

public:
	static CDummyObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pAtg) override;
	virtual void Free() override;
};

END