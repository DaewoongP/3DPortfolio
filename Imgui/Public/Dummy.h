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
	_float3 Get_PreToolScale() { return m_vScale; }
	_float3 Get_PreToolRotation() { return m_vRotation; }
	_float4 Get_PreToolTransform() { return m_vTransform; }
	void Set_PreToolScale(_float3 vScale) { m_vScale = vScale; }
	void Set_PreToolRotation(_float3 vRotation) { m_vRotation = vRotation; }
	void Set_PreToolTransform(_float4 vTransform) { m_vTransform = vTransform; }

public:
	CTransform* Get_TransformCom() const { return m_pTransformCom; }

protected:
	CTransform*			m_pTransformCom = { nullptr };

protected:
	_float3					m_vScale;
	_float3					m_vRotation;
	_float4					m_vTransform;

public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual void Free() override;
};

END