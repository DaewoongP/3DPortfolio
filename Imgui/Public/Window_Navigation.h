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
	// 현재 피킹상태를 변경해주기 위한 Set함수
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
	// 현재 네비메쉬가 피킹 되는지
	_bool							m_bPickNavigation = { false };
	// 현재 피킹하고있는 인덱스 (A,B,C)
	_uint							m_iCurrentPickIndex = { 0 };
	_float3							m_vCell[CCell::POINT_END];
	// 셀을 담고있는 벡터컨테이너 (인자 마다 float3를 3개씩 들고있음.)
	vector<_float3*>				m_Cells;
	// 셀의 인덱스 번호의 이름을 담고있는 벡터컨테이너
	vector<_char*>					m_CellIndices;
	// 현재 셀 리스트 박스의 인덱스
	_int							m_iCurrentListBoxIndex = { 0 };
	// 셀 값을 변경할 수 있게 처리해주는 bool
	_bool							m_bCellModifyMode = { false };

	// 현재 피킹한 버텍스 콜라이더를 공유하는 셀들
	// 버퍼의 셀 인덱스를 저장하기위해 map으로 설정
	vector<pair<_uint, CVIBuffer_Cell*>>	m_PickCellBuffers;
	// 현재 피킹한 버텍스 콜라이더를 사용하는 셀의 벡터
	vector<_uint>					m_PickCellIndex;

	_bool							m_isCellPicked = { false };
	_float4							m_vPickPos;
	
private:
	// 셀플래그를 담고있는 벡터 컨테이너.
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
	// 셀 변경모드가 켜져있을때 현재 네비게이션의 포지션을 변경할 수 있게하는 InputFloat3을 호출
	HRESULT CurrentNavigationPosition();
	HRESULT Navigation_List();
	// 셀의 렌더링을 변경하는 함수
	HRESULT Remake_Cells();
	HRESULT Delete_Cell();

	HRESULT NavigationSaveLoad();
	HRESULT NavigationSaveButton();
	HRESULT NavigationWrite_File(const _tchar* pPath);
	HRESULT NavigationLoadButton();
	HRESULT NavigationRead_File(const _tchar* pFileName);

private:
	// 셀의 방향 처리(CCW로 변경)
	void CCWSort_Cell(_float3* pPoints);

public:
	static CWindow_Navigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
	virtual void Free() override;
};

END