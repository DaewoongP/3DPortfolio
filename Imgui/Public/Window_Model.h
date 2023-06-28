#pragma once
#include "ImWindow.h"
#include "Terrain.h"
#include "Dummy.h"

BEGIN(Tool)

class CWindow_Model : public CImWindow
{
private:
	explicit CWindow_Model(ID3D11DeviceContext* pContext);
	virtual ~CWindow_Model() = default;

public:
	void Set_Picking(_bool bPick) { m_bPickMeshes = bPick; }

public:
	void Set_InputScale(_float3 vScale) { m_vScale = vScale; }
	void Set_InputRotation(_float3 vRotation) { m_vRotation = vRotation; }
	void Set_InputTransform(_float4 vTransform) { m_vTransform = vTransform; }

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double dTimeDelta) override;

private:
	ID3D11DeviceContext*	m_pContext = { nullptr };

private:
	CTerrain*				m_pTerrain = { nullptr };

private:
	// 현재 선택한 메쉬의 인덱스
	_int					m_iCur_Mesh = { 0 };
	// 마우스 피킹 bool 변수
	_bool					m_bPickMeshes = { false };
	// 현재 애니메이션 모델인지 아닌지 판단하는 radio 변수
	_int					m_iCurRadio = { 0 };

	_float3					m_vScale;
	_float3					m_vRotation;
	_float4					m_vTransform;
	// 현재 선택한 오브젝트의 기본 태그값
	// GameObject_NonAnimModel_
	// GameObject_AnimModel_
	_char					m_szObjectName[MAX_STR] = "";
	// 모델마다 태그에 이름을 다르게 주기위한 인덱스 값
	_uint					m_iCurrentModelIndex = { 0 };

private:
	// 툴에 있는 모든 애니메이션 모델 프로토타입을 저장한 맵컨테이너
	unordered_map<const _tchar*, CComponent*> m_AnimModelPrototypes;
	// 툴에 있는 모든 스태틱 모델 프로토타입을 저장한 맵컨테이너
	unordered_map<const _tchar*, CComponent*> m_NonAnimModelPrototypes;
	// 현재 프로토타입이 저장되어있는 맵과 같은 인덱스를 가진 이름을 보관한 벡터 컨테이너
	vector<const _char*>				m_AnimModelNames;
	// 현재 프로토타입이 저장되어있는 맵과 같은 인덱스를 가진 이름을 보관한 벡터 컨테이너
	vector<const _char*>				m_NonAnimModelNames;

private:
	HRESULT	Select_ModelFiles();
	HRESULT Setting_Transform();
	HRESULT MakeObject(_double dTimeDelta);

	HRESULT Initialize_Transforms();
	HRESULT Initialize_DummyTransforms();

	HRESULT MakeNonAnimModel(const _tchar* pName, _float4 vPickPos);
	HRESULT MakeAnimModel(const _tchar* pName, _float4 vPickPos);

	HRESULT MakeTag(CDummy::DUMMYTYPE eType);

	HRESULT MapSaveLoad();
	HRESULT MapSaveButton();
	HRESULT MapWrite_File(const _tchar* pPath);
	HRESULT MapLoadButton();
	HRESULT MapRead_File(const _tchar* pFileName);

public:
	static CWindow_Model* Create(ID3D11DeviceContext* pContext, void* pArg = nullptr);
	virtual void Free() override;
};

END