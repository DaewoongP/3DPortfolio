#pragma once
#include "VIBuffer_Instance.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Point_Instance final : public CVIBuffer_Instance
{
private:
	explicit CVIBuffer_Point_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CVIBuffer_Point_Instance(const CVIBuffer_Point_Instance& rhs);
	virtual ~CVIBuffer_Point_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	// 카메라의 위치를 객체의 로컬포지션으로 옮겨서 인스턴스 로컬행렬과 객체 기준 로컬의 카메라 좌표와 연산을한다.
	virtual void Tick(_float4x4* pInstanceMatrix, _bool isAlphaBlend = false, _fmatrix AlphaBlendObjectWorldMatrixInverse = XMMatrixIdentity());

public:
	HRESULT Make_Buffers();
	void Sort_AlphaBlend(_float4x4 * pInstanceMatrix, _fmatrix AlphaBlendObjectWorldMatrixInverse);

public:
	static CVIBuffer_Point_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END