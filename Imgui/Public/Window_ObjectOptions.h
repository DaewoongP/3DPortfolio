#pragma once
#include "ImWindow.h"
#include "Dummy.h"

BEGIN(Tool)

class CWindow_ObjectOptions : public CImWindow
{
private:
	explicit CWindow_ObjectOptions();
	virtual ~CWindow_ObjectOptions() = default;

public:
	// 옵션을 바꿀 객체 세팅
	void Set_CurrentDummy(CDummy::DUMMYTYPE eDummyType, class CDummy* pDummy);

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double dTimeDelta) override;

private:
	class CDummy*				m_pCurrentDummy = { nullptr };
	class CModel*				m_pCurrentModel = { nullptr };
	CDummy::DUMMYTYPE			m_eCurrentDummyType = { CDummy::DUMMY_END };

private: /* For.Animations */
	_char						m_szAnimationName[MAX_STR] = "";
	// 현재 모델의 전체 애니메이션 개수
	_uint						m_iNumAnimations = { 0 };
	// 애니메이션 중 현재 재생 중인 애니메이션의 인덱스
	_int						m_iAnimationIndex = { 0 };
	// 현재 재생 중인 애니메이션의 프레임 개수
	_uint						m_iAnimationFrames = { 0 };
	// 현재 재생 중인 프레임의 인덱스
	_uint						m_iCurrentAnimationFrame = { 0 };
	_bool						m_bPauseButton = { false };

private:
	class CAnimationNotify* m_pAnimationNotify = { nullptr };

private: /* For. Animations */
	HRESULT AnimationIndex();
	HRESULT AnimationPause();
	HRESULT AnimationSpeed(_double dTimeDelta);

	// 애니메이션의 특정 프레임에서 처리되는 값들
	HRESULT AnimationNotify(_double dTimeDelta);

private: /* For. Collider */
	HRESULT AddCollider();

private: /* For. Collider */
	_float3						m_ColliderExtents[3];

public:
	static CWindow_ObjectOptions* Create(void* pArg = nullptr);
	virtual void Free() override;
};

END