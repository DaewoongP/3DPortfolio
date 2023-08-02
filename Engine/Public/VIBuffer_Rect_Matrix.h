#pragma once
#include "VIBuffer_Instance.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Rect_Matrix final : public CVIBuffer_Instance
{
public:
	typedef struct tagInstanceDesc
	{
		const _float4x4* pRectMatrix;
	}INSTANCEDESC;

private:
	explicit CVIBuffer_Rect_Matrix(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CVIBuffer_Rect_Matrix(const CVIBuffer_Rect_Matrix& rhs);
	virtual ~CVIBuffer_Rect_Matrix() = default;

public:
	virtual HRESULT Initialize_Prototype(const INSTANCEDESC * pInstanceDesc, _uint iNumInstance);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double dTimeDelta) override;

private:
	INSTANCEDESC		m_InstanceDesc;

private:
	HRESULT Make_InstanceLogic(const INSTANCEDESC * pInstanceDesc);

public:
	static CVIBuffer_Rect_Matrix* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const INSTANCEDESC * pInstanceDesc, _uint iNumInstance);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END