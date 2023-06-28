#pragma once
#include "ImWindow.h"
#include "Cell.h"

BEGIN(Tool)

class CWindow_Navigation : public CImWindow
{
private:
	explicit CWindow_Navigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CWindow_Navigation() = default;

public:
	// ���� ��ŷ���¸� �������ֱ� ���� Set�Լ�
	void Set_Picking(_bool bPick) { m_bPickNavigation = bPick; }

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double dTimeDelta) override;

private:
	ID3D11Device*					m_pDevice = { nullptr };
	ID3D11DeviceContext*			m_pContext = { nullptr };

private:
	class CTerrain*					m_pTerrain = { nullptr };

private:
	// ���� �׺�޽��� ��ŷ �Ǵ���
	_bool							m_bPickNavigation = { false };
	// ���� ��ŷ�ϰ��ִ� �ε��� (A,B,C)
	_uint							m_iCurrentPickIndex = { 0 };
	_float3							m_vCell[CCell::POINT_END];
	// ���� ����ִ� ���������̳� (���� ���� float3�� 3���� �������.)
	vector<_float3*>				m_Cells;
	// ���� �ε��� ��ȣ�� �̸��� ����ִ� ���������̳�
	vector<_char*>					m_CellIndices;
	// ���� �� ����Ʈ �ڽ��� �ε���
	_int							m_iCurrentListBoxIndex = { 0 };
	// �� ���� ������ �� �ְ� ó�����ִ� bool
	_bool							m_bCellModifyMode = { false };

	// ���� ��ŷ�� ���ؽ� �ݶ��̴��� �����ϴ� ����
	// ������ �� �ε����� �����ϱ����� map���� ����
	vector<pair<_uint, CVIBuffer_Cell*>>	m_PickCellBuffers;
	// ���� ��ŷ�� ���ؽ� �ݶ��̴��� ����ϴ� ���� ����
	vector<_uint>					m_PickCellIndex;

	_bool							m_isCellPicked = { false };
	_float4							m_vPickPos;
	
private:
	// ���÷��׸� ����ִ� ���� �����̳�.
	vector<CELLFLAG>				m_eCellFlags;
	CELLFLAG						m_eCurrentCellFlag;
	_bool							m_isMoveCell = { false };
	_bool							m_isNullCell = { false };
	_bool							m_isWallCell = { false };
	_bool							m_isClimbCell = { false };
	_bool							m_isFallCell = { false };

private:
	HRESULT Pick_Navigation(_double dTimeDelta);
	HRESULT Pick_Terrain();
	// �� �����尡 ���������� ���� �׺���̼��� �������� ������ �� �ְ��ϴ� InputFloat3�� ȣ��
	HRESULT CurrentNavigationPosition();
	HRESULT Navigation_List();
	// ���� �������� �����ϴ� �Լ�
	HRESULT Remake_Cells();
	HRESULT Delete_Cell();

	HRESULT NavigationSaveLoad();
	HRESULT NavigationSaveButton();
	HRESULT NavigationWrite_File(const _tchar* pPath);
	HRESULT NavigationLoadButton();
	HRESULT NavigationRead_File(const _tchar* pFileName);

private:
	// ���� ���� ó��(CCW�� ����)
	void CCWSort_Cell(_float3* pPoints);

public:
	static CWindow_Navigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
	virtual void Free() override;
};

END