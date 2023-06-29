#pragma once
#include "Bounding.h"

BEGIN(Engine)

class ENGINE_DLL CBounding_OBB final : public CBounding
{
public:
	typedef struct tagBoundingOBBDesc : public CBounding::tagBoundingDesc
	{
		_float3		vExtents;
		_float3		vRotation;
	}BOUNDINGOBBDESC;

private:
	explicit CBounding_OBB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CBounding_OBB(const CBounding_OBB& rhs);
	virtual ~CBounding_OBB() = default;

public:
	virtual _float3 Get_CenterPosition() const  override { return m_pOBB->Center; }
	virtual void Set_BoundingDesc(void* pBoundingDesc) override;

public:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pBoundingDesc);
	virtual void Tick(_fmatrix WorldMatrix) override;

public:
	virtual _bool Intersects(CBounding* pOtherBounding, _float3* pCollisionBox);

#ifdef _DEBUG
public:
	virtual HRESULT Render(_fvector vColor = COLLIDERCOLOR) override;
#endif // _DEBUG

private:
	// 행렬 변환이 중첩되어 처리될 수 있기 때문에 필요한 원본 객체.
	DirectX::BoundingOrientedBox* m_pOBB_Original = { nullptr };
	// 행렬 변환을 직접적으로 받고 렌더링하는 객체.
	DirectX::BoundingOrientedBox* m_pOBB = { nullptr };

public:
	static CBounding_OBB* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CBounding* Clone(void* pBoundingDesc) override;
	virtual void Free() override;
};

END