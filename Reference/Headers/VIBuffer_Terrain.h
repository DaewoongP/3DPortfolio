#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Terrain final : public CVIBuffer
{
private:
	explicit CVIBuffer_Terrain(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	explicit CVIBuffer_Terrain(const CVIBuffer_Terrain& rhs);
	virtual ~CVIBuffer_Terrain() = default;

public:
	const _float3* Get_PosArray() const { return m_pPos; }
	const _uint* Get_Index() const { return m_pIndex; }

public:
	virtual HRESULT Initialize_Prototype(const _tchar * pHeightMap);
	virtual HRESULT Initialize_Prototype(_uint iTerrainSizeX, _uint iTerrainSizeY);
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT RemakeTerrain(const _tchar * pHeightMap);
	HRESULT RemakeTerrain(_uint iTerrainSizeX, _uint iTerrainSizeY);

private:
	_float3*			m_pPos = { nullptr };
	_uint*				m_pIndex = { nullptr };

public:
	static CVIBuffer_Terrain* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pHeightMap);
	static CVIBuffer_Terrain* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, _uint iTerrainSizeX, _uint iTerrainSizeY);	
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END