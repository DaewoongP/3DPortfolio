#pragma once
#include "ImWindow.h"
#include "Dummy.h"

BEGIN(Tool)

class CWindow_ObjectOptions : public CImWindow
{
public:
	struct Funcs // graph style
	{
		static float Sin(void*, int i) { return sinf(i * 0.1f); }
		static float Saw(void*, int i) { return (i & 1) ? 1.0f : -1.0f; }
		static _float FrameSpeedSaw(void* pArg, _int i) 
		{
			vector<_float> FrameSpeed;
			FrameSpeed = *reinterpret_cast<vector<_float>*>(pArg);
			if (0 == FrameSpeed.size())
				return 0;
			return FrameSpeed[i];
		}
	};

private:
	explicit CWindow_ObjectOptions();
	virtual ~CWindow_ObjectOptions() = default;

public:
	// 현재 애니메이션의 프레임 스피드가 들어있는 벡터컨테이너 반환
	vector<_float> Get_FrameSpeeds() const { return m_FrameSpeeds; }
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
	// 현재 재생 중인 애니메이션의 프레임 속도를 담은 벡터 컨테이너
	vector<_float>				m_FrameSpeeds;

	// 히스토그램을 그리기 위한 함수포인터 변수
	_float						(*m_FrameSpeedFunc)(void*, _int);

private: /* For. Animations */
	HRESULT AnimationIndex();
	HRESULT AnimationPause();
	HRESULT AnimationSpeed();

private: /* For. Collider */
	HRESULT AddCollider();

public:
	static CWindow_ObjectOptions* Create(void* pArg = nullptr);
	virtual void Free() override;
};

END