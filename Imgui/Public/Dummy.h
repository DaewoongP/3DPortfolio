#pragma once
#include "GameObject.h"
#include "Tool_Defines.h"
#include "Bounding_OBB.h"
#include "Bounding_AABB.h"
#include "Bounding_Sphere.h"

BEGIN(Tool)

class CDummy abstract : public CGameObject
{
public:
	enum DUMMYTYPE { DUMMY_NONANIM, DUMMY_ANIM, DUMMY_END };

public:
	typedef struct tagDummyObjectDesc
	{
		_tchar pModelPrototypeTag[MAX_STR] = TEXT("");
		_float4 vPosition = _float4(0.f, 0.f, 0.f, 1.f);
	}OBJECTDESC;

protected:
	explicit CDummy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CDummy(const CDummy& rhs);
	virtual ~CDummy() = default;

public:
	// ���� ������Ʈ�� ���� ������Ÿ�� �±׿� �ʱ������ǰ��� ���� ����ü ��ȯ
	OBJECTDESC Get_ObjectDesc() { return m_ObjectDesc; }
	// ������ ������ ������ �� ��ȯ
	_float3 Get_PreToolScale() { return m_vPreScale; }
	// ������ ������ ������ �� ��ȯ
	_float3 Get_PreToolRotation() { return m_vPreRotation; }
	// ������ ������ ������ �� ��ȯ
	_float4 Get_PreToolTransform() { return m_vPreTransform; }
	// ������ ������ ������ �� ��ȯ
	_uint Get_PreToolAnimationIndex() { return m_iPreAnimationIndex; }
	// ������ ������ ������ �� ��ȯ
	_double Get_PreToolAnimationSpeed() { return m_dPreAnimationSpeed; }
	// ������ �ٽ� Ȯ���ϱ� ���� ���� ����
	void Set_PreToolScale(_float3 vScale) { m_vPreScale = vScale; }
	// ������ �ٽ� Ȯ���ϱ� ���� ���� ����
	void Set_PreToolRotation(_float3 vRotation) { m_vPreRotation = vRotation; }
	// ������ �ٽ� Ȯ���ϱ� ���� ���� ����
	void Set_PreToolTransform(_float4 vTransform) { m_vPreTransform = vTransform; }
	// ������ �ٽ� Ȯ���ϱ� ���� ���� ����
	void Set_PreToolAnimationIndex(_uint iIndex) { m_iPreAnimationIndex = iIndex; }
	// ������ �ٽ� Ȯ���ϱ� ���� ���� ����
	void Set_PreToolAnimationSpeed(_double dSpeed) { m_dPreAnimationSpeed = dSpeed; }

public:
	virtual void Tick(_double dTimeDelta) override;
	virtual HRESULT Render() override;

public:
	CTransform* Get_TransformCom() const { return m_pTransformCom; }
	CCollider* Get_ColliderCom() const { return m_pColliderCom; }
	CBounding_Sphere::BOUNDINGSPHEREDESC Get_SphereDesc() const { return m_pBoundingSphereDesc; }
	CBounding_OBB::BOUNDINGOBBDESC		 Get_OBBDesc() const { return m_pBoundingOBBDesc; }
	CBounding_AABB::BOUNDINGAABBDESC	 Get_AABBDesc() const { return m_pBoundingAABBDesc; }
	void Set_ColliderCom(class CCollider* pCollider) { m_pColliderCom = pCollider; }
	_bool Get_ColliderInMap() { return m_UseColliderInMap; }
	void Set_ColliderInMap(_bool bUse) {
		if (nullptr != m_pColliderCom && true == bUse)
			m_UseColliderInMap = bUse;
		else if (false == bUse)
			m_UseColliderInMap = bUse;
		else
			MSG_BOX("Set Collider First");
	}
	void Set_BoundingDesc(CCollider::TYPE eType, void* pBoundingDesc);
	
protected:
	_bool					m_UseColliderInMap = { false };
	CCollider*				m_pColliderCom = { nullptr };

	CCollider::TYPE			m_eColliderType = { CCollider::TYPE_END };

	CBounding_Sphere::BOUNDINGSPHEREDESC	m_pBoundingSphereDesc;
	CBounding_OBB::BOUNDINGOBBDESC			m_pBoundingOBBDesc;
	CBounding_AABB::BOUNDINGAABBDESC		m_pBoundingAABBDesc;

protected:
	OBJECTDESC				m_ObjectDesc;
	_float3					m_vPreScale;
	_float3					m_vPreRotation;
	_float4					m_vPreTransform;
	_uint					m_iPreAnimationIndex = { 0 };
	_double					m_dPreAnimationSpeed = { 1.0 };

public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual void Free() override;
};

END