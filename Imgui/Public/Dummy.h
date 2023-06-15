#pragma once
#include "GameObject.h"
#include "Tool_Defines.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Tool)

class CDummy abstract : public CGameObject
{
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
	_float3 Get_PreToolScale() { return m_vPreScale; }
	_float3 Get_PreToolRotation() { return m_vPreRotation; }
	_float4 Get_PreToolTransform() { return m_vPreTransform; }
	_uint Get_PreToolAnimationIndex() { return m_iPreAnimationIndex; }
	_double Get_PreToolAnimationSpeed() { return m_dPreAnimationSpeed; }
	void Set_PreToolScale(_float3 vScale) { m_vPreScale = vScale; }
	void Set_PreToolRotation(_float3 vRotation) { m_vPreRotation = vRotation; }
	void Set_PreToolTransform(_float4 vTransform) { m_vPreTransform = vTransform; }
	void Set_PreToolAnimationIndex(_uint iIndex) { m_iPreAnimationIndex = iIndex; }
	void Set_PreToolAnimationSpeed(_double dSpeed) { m_dPreAnimationSpeed = dSpeed; }

public:
	CTransform* Get_TransformCom() const { return m_pTransformCom; }

protected:
	CTransform*			m_pTransformCom = { nullptr };

protected:
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