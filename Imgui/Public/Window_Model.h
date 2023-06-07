#pragma once
#include "ImWindow.h"
#include "Terrain.h"

BEGIN(Tool)

class CWindow_Model : public CImWindow
{
private:
	explicit CWindow_Model();
	virtual ~CWindow_Model() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double dTimeDelta) override;

private:
	CTerrain*				m_pTerrain = { nullptr };

private:
	_uint					m_iDummyNum = { 0 };
	_uint					m_iMaxSelection = { 20 };
	_int					m_iCur_Mesh = { 1 };
	_bool					m_bPickMeshes = { false };

private:
	map<string, string>		m_SelectionMap;
	unordered_map<const _tchar*, CComponent*> m_ModelPrototypes;

private:
	HRESULT	Select_MeshFiles();
	HRESULT Open_Dialog();
	HRESULT Setting_Transform();
	HRESULT MakeObject(_double dTimeDelta);

public:
	static CWindow_Model* Create(void* pArg = nullptr);
	virtual void Free() override;
};

END