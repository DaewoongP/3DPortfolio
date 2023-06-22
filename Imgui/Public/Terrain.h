#pragma once
#include "Tool_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CCollider;
class CRenderer;
class CTransform;
class CVIBuffer_Cell;
class CVIBuffer_Terrain;
END

BEGIN(Tool)

class CTerrain : public CGameObject
{
private:
	explicit CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CTerrain(const CTerrain& rhs);
	virtual ~CTerrain() = default;

public:
	// 터레인에 저장된 텍스처 개수
	_uint Get_NumTextures();
	// 텍스처 인덱스 저장
	void Set_TextureIndex(_int iIndex);
	void Set_WireFrame(_bool isWireFrame) { m_bIsWireFrame = isWireFrame; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double dTimeDelta) override;
	virtual void Late_Tick(_double dTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT RemakeTerrain(const _tchar* pHeightMap);
	HRESULT RemakeTerrain(_uint iSizeX, _uint iSizeY);
	HRESULT RemakeTerrain(_uint iTextureIndex);
	HRESULT RemakeCells(vector<_float3*>& Cells);
	HRESULT PickingOnTerrain(_Inout_ _float4* vPickPos);
	// 피킹이 이미 생성되어있는 셀의 버텍스에 되었는지 체크하여
	// 피킹 되었을경우 true와 해당 포지션을 반환한다.
	// 2번째 인자 :  클릭한 버텍스가 존재하는 셀의 인덱스들
	// 3번째 인자 : 클릭한 버텍스가 존재하는 셀의 버퍼, 버퍼의 버텍스 ABC 중 해당하는 인덱스를 맵으로 반환
	_bool IsPickingOnCell(_Inout_ _float4* vPickPos, _Inout_ vector<_uint>& CellIndex, _Inout_ vector<pair<_uint, CVIBuffer_Cell*>>& PickCells);

private:
	CShader*				m_pShaderCom = { nullptr };
	CTexture*				m_pTextureCom = { nullptr };
	CRenderer*				m_pRendererCom = { nullptr };
	CTransform*				m_pTransformCom = { nullptr };
	CVIBuffer_Terrain*		m_pTerrainCom = { nullptr };

	CShader*				m_pCellShaderCom = { nullptr };
	vector<CVIBuffer_Cell*> m_Cells;

private:
	_uint					m_iTextureIndex = { 0 };
	_bool					m_bIsWireFrame = { false };

private:
	HRESULT		Add_Component();
	HRESULT		SetUp_ShaderResources();

public:
	static CTerrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END