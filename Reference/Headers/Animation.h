#pragma once
#include "Model.h"

BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	explicit CAnimation();
	explicit CAnimation(const CAnimation& rhs);
	virtual ~CAnimation() = default;

public:
	// ���� �ִϸ��̼� �̸� ��ȯ
	const _tchar* Get_AnimationName() const { return m_szName; }
	// �ִϸ��̼��� ������ ���� ��ȯ
	_uint Get_AnimationFrames() { return m_iAnimationFrames; }
	// ä�� �� Ű������ �ִ�ġ ����, **���� �ִϸ��̼��� ������ ��ȯ**
	_uint Get_CurrentAnimationFrame() { return m_ChannelCurrentKeyFrames[m_iMaxFrameChannelIndex]; }
	vector<NOTIFY> Get_CurrentAnimationNotify() { return m_AnimationNotify; }
	// ������ �ε����� �ش��ϴ� ���ǵ尪 ����.
	void Set_FrameSpeed(_uint iFrameIndex, _float fSpeed) {
		m_AnimationNotify[iFrameIndex].fSpeed = fSpeed;
	}
	// ������ �ε����� �ش��ϴ� ��Ƽ���� ī�޶� �� ����
	void Set_FrameCamera(_uint iFrameIndex, _float4 vEye, _float4 vAt) {
		m_AnimationNotify[iFrameIndex].vEye = vEye;
		m_AnimationNotify[iFrameIndex].vAt = vAt;
	}
	void Set_CurrentKeyFrameIndex(CModel::BONES& Bones, _uint iKeyFrameIndex);
	void Set_Pause(_bool isPause) { m_isPaused = isPause; }
	void Set_TickPerSecond(_double dMultiply) 
	{
		if (0 >= dMultiply)
			return;

		m_dTickPerSecond = m_dOriginTickPerSecond * dMultiply;
	}
	void Set_Loop(_bool isLoop) { m_isLoop = isLoop; }
	// �ִϸ��̼��� �ʱ�ȭ �ϴ� �Լ�
	void Reset() 
	{ 
		for (auto& iCurrentKeyFrame : m_ChannelCurrentKeyFrames)
			iCurrentKeyFrame = 0;
		m_dTimeAcc = 0.0;
	}
	void TimeAccReset() { m_dTimeAcc = 0.0; }

public:
	HRESULT Initialize(Engine::ANIMATION* pAnimation, const CModel::BONES& Bones);
	ANIMATIONFLAG Invalidate_TransformationMatrix(CModel::BONES& Bones, _double TimeDelta);
	void Invalidate_Camera(class CCamera* pCamera, class CTransform* pPlayerTransform, _double dTimeDelta);
	HRESULT SetUp_AnimationNotifies(vector<NOTIFY> Notifies);
	ANIMATIONFLAG Lerp_TransformMatrix(CModel::BONES& Bones, class CAnimation* pCurrentAnimation, _double TimeDelta, _double dDuration = 0.2);

public:
	_tchar						m_szName[MAX_STR] = TEXT("");
	// �ִϸ��̼��� ����ϴ� ä��(��)�� ����
	_uint						m_iNumChannels = { 0 };
	// ä���� ����ִ� ���� �����̳�
	vector<class CChannel*>		m_Channels;
	// �� ä���� ���� Ű������ �ε���
	vector<_uint>				m_ChannelCurrentKeyFrames;

	_double						m_dDuration = { 0.2 };
	_double						m_dOriginTickPerSecond = { 0.0 };
	_double						m_dTickPerSecond = { 0.0 };
	_double						m_dTimeAcc = { 0.0 };

	_bool						m_isLoop = { false };
	_bool						m_isPaused = { false };

	// ���� �ִϸ��̼ǿ� �ش��ϴ� ä���� ������ �� ���� �������� ���� ä���� �ε���
	_uint						m_iMaxFrameChannelIndex = { 0 };
	// �ִϸ��̼��� �ִ� ������ ����
	_uint						m_iAnimationFrames = { 0 };
	// ���� �ִϸ��̼ǿ� �ش��ϴ� ä���� ������ �� ���� ū �������� �������� ����� ó��.
	// �� ������ ���� Notify�� ����
	vector<NOTIFY>				m_AnimationNotify;

public:
	static CAnimation* Create(Engine::ANIMATION* pAnimation, const CModel::BONES& Bones);
	CAnimation* Clone();
	virtual void Free() override;
};

END