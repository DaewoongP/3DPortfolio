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
	// ������ �ε����� �ش��ϴ� ���ǵ尪 ����.
	void Set_FrameSpeed(_uint iFrameIndex, _float fSpeed);
	void Set_CurrentKeyFrameIndex(CModel::BONES& Bones, _uint iKeyFrameIndex);
	void Set_Pause(_bool isPause) { m_isPaused = isPause; }
	void Set_TickPerSecond(_double dMultiply) 
	{
		if (0 >= dMultiply)
			return;

		m_dTickPerSecond = m_dOriginTickPerSecond * dMultiply;
	}
	// ī�޶� eye, at���� �����ӿ� ����
	void Set_CameraValueInFrame(_uint iFrameIndex, _float3 vEye, _float3 vAt);

public:
	HRESULT Initialize(Engine::ANIMATION* pAnimation, const CModel::BONES& Bones);
	void Invalidate_TransformationMatrix(CModel::BONES& Bones, _double TimeDelta);

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
	// �� ������ ���� ���ǵ带 �����ϴ� ����.
	vector<_float>				m_AnimationFrameSpeeds;

public:
	static CAnimation* Create(Engine::ANIMATION* pAnimation, const CModel::BONES& Bones);
	CAnimation* Clone();
	virtual void Free() override;
};

END