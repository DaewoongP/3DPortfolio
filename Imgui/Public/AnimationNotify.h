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
	// ���� �ִϸ��̼��� �ε���
	_uint				m_iAnimationIndex = { 0 };
	// ���� �ִϸ��̼� ��ü ������ ����
	_uint				m_iFrames = { 0 };
	// ���� �������� �ε���
	_uint				m_iCurrentFrameIndex = { 0 };
	_uint				m_iLerpFirstFrameIndex = { 0 };
	_uint				m_iLerpLastFrameIndex = { 0 };
	_bool				m_bCamera = { false };

	_bool				m_bTestCamera = { false };
	// ī�޶� ���º���
	_float4				m_vEye;
	_float4				m_vAt;
	
	// ���� ���� ���� �������
	_float4x4			m_DummyMatrix;
	// Frame�� ������ �Ǿ����� Ȯ��
	vector<_bool>		m_SetFrames;	// ���� ��� ���� �ִϸ��̼��� ������ �ӵ��� ���� ���� �����̳�
	vector<NOTIFY>		m_FrameNotify;

	// ������׷��� �׸��� ���� �Լ������� ����
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