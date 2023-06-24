#pragma once
#include "ImWindow.h"
#include "Tool_Defines.h"

BEGIN(Tool)

class CAnimationNotify final : public CImWindow
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
	explicit CAnimationNotify();
	virtual ~CAnimationNotify() = default;

public:
	void Set_CurrentAnimationObject(class CAnimModel* pAnimDummy, class CModel* pModel);

public:
	HRESULT Initialize(void* pArg);
	void Tick(_double dTimeDelta);

private:
	class CAnimModel*	m_pCurrentAnimDummy = { nullptr };
	class CModel*		m_pCurrentModelCom = { nullptr };
	class CNonAnimModel* m_pToolCamera = { nullptr };
	class CTransform*	m_pToolCameraTrans = { nullptr };
	// 현재 애니메이션의 인덱스
	_uint				m_iAnimationIndex = { 0 };
	// 현재 애니메이션 전체 프레임 개수
	_uint				m_iFrames = { 0 };
	// 현재 프레임의 인덱스
	_uint				m_iCurrentFrameIndex = { 0 };
	_uint				m_iLerpFirstFrameIndex = { 0 };
	_uint				m_iLerpLastFrameIndex = { 0 };
	_bool				m_bCamera = { false };

	_bool				m_bTestCamera = { false };
	// 카메라 상태벡터
	_float4				m_vEye;
	_float4				m_vAt;
	
	// 현재 모델의 원래 상태행렬
	_float4x4			m_DummyMatrix;
	// Frame이 세팅이 되었는지 확인
	vector<_bool>		m_SetFrames;	// 현재 재생 중인 애니메이션의 프레임 속도를 담은 벡터 컨테이너
	vector<NOTIFY>		m_FrameNotify;

	// 히스토그램을 그리기 위한 함수포인터 변수
	_float(*m_FrameSpeedFunc)(void*, _int);

private:
	HRESULT NotifyCamera();
	HRESULT CameraSetLerp();
	HRESULT LerpReset();
	HRESULT TestCamera();
	HRESULT NotifySpeed();

	HRESULT InputFrameIndex();

public:
	static CAnimationNotify* Create(void* pArg = nullptr);
	virtual void Free() override;
};

END