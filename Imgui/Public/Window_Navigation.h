#pragma once
#include "ImWindow.h"
#include "Cell.h"

BEGIN(Tool)

class CWindow_Navigation : public CImWindow
{
private:
	explicit CWindow_Navigation(ID3D11DeviceContext* pContext);
	virtual ~CWindow_Navigation() = default;

public:
	void Set_Picking(_bool bPick) { m_bPickNavigation = bPick; }

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double dTimeDelta) override;
	
private:
	ID3D11DeviceContext*			m_pContext = { nullptr };

private:
	class CTerrain*					m_pTerrain = { nullptr };

private:
	_bool							m_bPickNavigation = { false };
	_uint							m_iCurrentPickIndex = { 0 };
	_float3							m_vCell[CCell::POINT_END];
	vector<_float3*>				m_Cells;

	vector<_char*>					m_CellIndices;

	_int							m_iCurrentListBoxIndex = { 0 };

	_bool							m_bCellModifyMode = { false };
private:
	HRESULT Pick_Navigation();
	HRESULT CurrentNavigationPosition();
	HRESULT Navigation_List();
	HRESULT Remake_Cells();
	HRESULT Delete_Cell();

	HRESULT NavigationSaveLoad();
	HRESULT NavigationSaveButton();
	HRESULT NavigationWrite_File(const _tchar* pPath);
	HRESULT NavigationLoadButton();
	HRESULT NavigationRead_File(const _tchar* pFileName);

private:
	void CCWSort_Cell(_float3* pPoints);

public:
	static CWindow_Navigation* Create(ID3D11DeviceContext* pContext, void* pArg = nullptr);
	virtual void Free() override;
};

END