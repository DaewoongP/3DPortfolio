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
	// 저장할 빛 구조체 정보
	vector<pair<class CCollider*, CLight::LIGHTDESC>>		m_Lights;
	CLight::LIGHTDESC			m_LightDesc;
	// 빛 구조체 인덱스
	vector<_char*>				m_LightIndices;
	_int						m_iCurrentListBoxIndex = { 0 };
	// 자동적으로 증가하면서 빛의 인덱스를 다르게 처리하여 넣어주기 위한변수
	_uint						m_iCurrentLightIndex = { 0 };
	// 점광원을 화면에 띄울것인지 디렉션 광원을 띄울것인지
	_bool						m_isLight = { false };
	_bool						m_bClearButton = { false };

	_bool						m_isPickMode = { false };
	_bool						m_isModifyMode = { false };
	_int						m_iPickingIndex = { -1 };
	
private:
	// 피킹을 통해 포지션만 갱신하기 위한 함수.
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

// 직접 값대입 ,현재 카메라 포지션을 가져와서 출력해줌
// 포지션 세팅, 바운딩스피어 씌우기
// 무조건 점광원\
// 빛 구조체 채울 값 세팅
// 저장 및 불러오기