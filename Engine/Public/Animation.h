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
	_uint Get_AnimationFrames();
	// ���� �ִϸ��̼��� ������ ��ȯ
	// ä�� �� Ű������ �ִ�ġ ����.
	_uint Get_CurrentAnimationFrame();
	vector<NOTIFY> Get_CurrentAnimationNotify() const { return m_AnimationNotify; }
	// ������ �ε����� �ش��ϴ� ���ǵ尪 ����.
	void Set_FrameSpeed(_uint iFrameIndex, _float fSpeed);
	// ������ �ε����� �ش��ϴ� ��Ƽ���� ī�޶� �� ����
	void Set_FrameCamera(_uint iFrameIndex, _float4 vEye, _float4 vAt);
	void Set_CurrentKeyFrameIndex(CModel::BONES& Bones, _uint iKeyFrameIndex);
	void Set_Pause(_bool isPause) { m_isPaused = isPause; }
	void Set_TickPerSecond(_double dMultiply) 
	{
		if (0 >= dMultiply)
			return;

		m_dTickPerSecond = m_dOriginTickPerSecond * dMultiply;
	}

public:
	HRESULT Initialize(Engine::ANIMATION* pAnimation, const CModel::BONES& Bones);
	void Invalidate_TransformationMatrix(CModel::BONES& Bones, _double TimeDelta);
	void Invalidate_Camera(class CCamera* pCamera);

public:
	_tchar						m_szName[MAX_STR] = TEXT("");
	// �ִϸ��̼��� ����ϴ� ä��(��)�� ����
	_uint						m_iNumChannels = { 0 };
	// ä���� ����ִ� ���� �����̳�
	vector<class CChannel*>		m_Channels;
	// �� ä���� ���� Ű������ �ε���
	vector<_uint>				m_ChannelCurrentKeyFrames;

	_double						m_dDuration = { 0.0 };
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