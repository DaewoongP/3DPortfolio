#pragma once
#include "VIBuffer_Instance.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Point_Instance_World final : public CVIBuffer_Instance
{
private:
	explicit CVIBuffer_Point_Instance_World(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CVIBuffer_Point_Instance_World(const CVIBuffer_Point_Instance_World& rhs);
	virtual ~CVIBuffer_Point_Instance_World() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iNumInstance);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float4x4* InstanceMatrix);

public:
	static CVIBuffer_Point_Instance_World* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,  _uint iNumInstance);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END