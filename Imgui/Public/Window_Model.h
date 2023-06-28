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
	// ���� ������ �޽��� �ε���
	_int					m_iCur_Mesh = { 0 };
	// ���콺 ��ŷ bool ����
	_bool					m_bPickMeshes = { false };
	// ���� �ִϸ��̼� ������ �ƴ��� �Ǵ��ϴ� radio ����
	_int					m_iCurRadio = { 0 };

	_float3					m_vScale;
	_float3					m_vRotation;
	_float4					m_vTransform;
	// ���� ������ ������Ʈ�� �⺻ �±װ�
	// GameObject_NonAnimModel_
	// GameObject_AnimModel_
	_char					m_szObjectName[MAX_STR] = "";
	// �𵨸��� �±׿� �̸��� �ٸ��� �ֱ����� �ε��� ��
	_uint					m_iCurrentModelIndex = { 0 };

private:
	// ���� �ִ� ��� �ִϸ��̼� �� ������Ÿ���� ������ �������̳�
	unordered_map<const _tchar*, CComponent*> m_AnimModelPrototypes;
	// ���� �ִ� ��� ����ƽ �� ������Ÿ���� ������ �������̳�
	unordered_map<const _tchar*, CComponent*> m_NonAnimModelPrototypes;
	// ���� ������Ÿ���� ����Ǿ��ִ� �ʰ� ���� �ε����� ���� �̸��� ������ ���� �����̳�
	vector<const _char*>				m_AnimModelNames;
	// ���� ������Ÿ���� ����Ǿ��ִ� �ʰ� ���� �ε����� ���� �̸��� ������ ���� �����̳�
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