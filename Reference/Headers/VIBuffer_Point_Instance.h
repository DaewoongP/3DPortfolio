#pragma once
#include "VIBuffer_Instance.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Point_Instance final : public CVIBuffer_Instance
{
public:
	typedef struct tagInstanceDesc
	{
		// 떨어지는 최대 시작점
		_float		fHeight;
		// 파티클 전체의 바운딩박스
		_float3		vExtents;
		// 최소 스피드 X ~ 최대 스피드 Y
		_uint2		vSpeed;
		// 생존 시간
		_float		fLifeTime;
	}INSTANCEDESC;

private:
	explicit CVIBuffer_Point_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CVIBuffer_Point_Instance(const CVIBuffer_Point_Instance& rhs);
	virtual ~CVIBuffer_Point_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype(const INSTANCEDESC * pInstanceDesc, _uint iNumInstance);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float4x4* InstanceMatrix);

private:
	INSTANCEDESC				m_InstanceDesc;

public:
	static CVIBuffer_Point_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const INSTANCEDESC* pInstanceDesc, _uint iNumInstance);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END