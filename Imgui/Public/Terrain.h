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
	// �ͷ��ο� ����� �ؽ�ó ����
	_uint Get_NumTextures();
	// �ؽ�ó �ε��� ����
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
	// ��ŷ�� �̹� �����Ǿ��ִ� ���� ���ؽ��� �Ǿ����� üũ�Ͽ�
	// ��ŷ �Ǿ������ true�� �ش� �������� ��ȯ�Ѵ�.
	// 2��° ���� :  Ŭ���� ���ؽ��� �����ϴ� ���� �ε�����
	// 3��° ���� : Ŭ���� ���ؽ��� �����ϴ� ���� ����, ������ ���ؽ� ABC �� �ش��ϴ� �ε����� ������ ��ȯ
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