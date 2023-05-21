#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Terrain final : public CVIBuffer
{
protected:
	explicit CVIBuffer_Terrain(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	explicit CVIBuffer_Terrain(const CVIBuffer_Terrain& rhs);
	virtual ~CVIBuffer_Terrain() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iTerrainSizeX, _uint iTerrainSizeY);
	virtual HRESULT Initialize(void* pArg) override;

public:
	static CVIBuffer_Terrain* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, _uint iTerrainSizeX, _uint iTerrainSizeY);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END