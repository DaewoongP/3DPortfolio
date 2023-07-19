#pragma once
#include "ImWindow.h"
#include "Light.h"

BEGIN(Engine)
class CRenderer;
class CCollider;
END

BEGIN(Tool)

class CWindow_Light final : public CImWindow
{
private:
	explicit CWindow_Light(ID3D11DeviceContext* pContext);
	virtual ~CWindow_Light() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double dTimeDelta) override;

private:
	ID3D11DeviceContext*		m_pContext = { nullptr };

private:
	class CTerrain*				m_pTerrain  = { nullptr };
	class CRenderer*			m_pRenderer = { nullptr };
	class CCollider*			m_pOriginCollider = { nullptr };
	// ������ �� ����ü ����
	vector<pair<class CCollider*, CLight::LIGHTDESC>>		m_Lights;
	CLight::LIGHTDESC			m_LightDesc;
	// �� ����ü �ε���
	vector<_char*>				m_LightIndices;
	_int						m_iCurrentListBoxIndex = { 0 };
	// �ڵ������� �����ϸ鼭 ���� �ε����� �ٸ��� ó���Ͽ� �־��ֱ� ���Ѻ���
	_uint						m_iCurrentLightIndex = { 0 };
	// �������� ȭ�鿡 �������� �𷺼� ������ ��������
	_bool						m_isLight = { false };
	_bool						m_bClearButton = { false };

	_bool						m_isPickMode = { false };
	_bool						m_isModifyMode = { false };
	_int						m_iPickingIndex = { -1 };
	
private:
	// ��ŷ�� ���� �����Ǹ� �����ϱ� ���� �Լ�.
	HRESULT Find_Position();
	HRESULT SetUp_LightDesc();
	HRESULT Delete_LightDesc();
	HRESULT Pick_Light(_double dTimeDelta);

	HRESULT LightSaveLoad();
	HRESULT LightSaveButton();
	HRESULT LightWrite_File(const _tchar* pPath);
	HRESULT LightLoadButton();
	HRESULT LightRead_File(const _tchar* pFileName);

private:
	void Add_DirectionalLight();
	void Remake_Lights();
	_int isPickLight();

public:
	static CWindow_Light* Create(ID3D11DeviceContext* pContext, void* pArg = nullptr);
	virtual void Free() override;
};

END;

// ���� ������ ,���� ī�޶� �������� �����ͼ� �������
// ������ ����, �ٿ�����Ǿ� �����
// ������ ������\
// �� ����ü ä�� �� ����
// ���� �� �ҷ�����