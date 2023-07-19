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
	explicit CWindow_Light();
	virtual ~CWindow_Light() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double dTimeDelta) override;

private:
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
private:
	HRESULT SetUp_LightDesc();
	HRESULT Delete_LightDesc();

public:
	static CWindow_Light* Create(void* pArg = nullptr);
	virtual void Free() override;
};

END;

// ���� ������ ,���� ī�޶� �������� �����ͼ� �������
// ������ ����, �ٿ�����Ǿ� �����
// ������ ������\
// �� ����ü ä�� �� ����
// ���� �� �ҷ�����