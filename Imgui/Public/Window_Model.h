#pragma once
#include "ImWindow.h"
#include "Terrain.h"

BEGIN(Tool)

class CWindow_Model : public CImWindow
{
public:
	enum RADIO { NONANIM, ANIM, RADIO_END };

private:
	explicit CWindow_Model();
	virtual ~CWindow_Model() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double dTimeDelta) override;

private:
	CTerrain*				m_pTerrain = { nullptr };
	CGameObject*			m_pLastObject = { nullptr };

private:
	_uint					m_iDummyNum = { 0 };
	_uint					m_iMaxSelection = { 20 };
	_int					m_iCur_Mesh = { 0 };
	_bool					m_bPickMeshes = { false };
	_int					m_iCurRadio = { 0 };
	_float3					m_vScale;
	_float3					m_vRotate;
	_float4					m_vTransform;

private:
	map<string, string>					m_SelectionMap;
	unordered_map<const _tchar*, CComponent*> m_AnimModelPrototypes;
	unordered_map<const _tchar*, CComponent*> m_NonAnimModelPrototypes;
	vector<const _char*>				m_NonAnimModelItems;
	vector<const _char*>				m_AnimModelItems;

private:
	HRESULT	Select_MeshFiles();
	HRESULT Open_Dialog();
	HRESULT Setting_Transform();
	HRESULT MakeObject(_double dTimeDelta);

	HRESULT Initialize_Transforms();

	HRESULT MakeAnimModel(_float4 vPickPos);
	HRESULT MakeNonAnimModel(_float4 vPickPos);

public:
	static CWindow_Model* Create(void* pArg = nullptr);
	virtual void Free() override;
};

END