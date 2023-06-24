#pragma once
#include "ImWindow.h"
#include "Tool_Defines.h"

BEGIN(Tool)

class CAnimationNotify final : public CImWindow
{
private:
	explicit CAnimationNotify();
	virtual ~CAnimationNotify() = default;

public:
	void Set_CurrentAnimationObject(class CAnimModel* pAnimDummy, class CModel* pModel);
	void Set_CurrentIndex(_uint iAnimaitonIndex, _uint iFrameIndex);

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
	// ���� �������� �ε���
	_uint				m_iFrameIndex = { 0 };
	_bool				m_bCamera = { false };
	// ī�޶� ���º���
	_float4				m_vEye;
	_float3				m_vAt;
	

private:
	HRESULT NotifyCamera();
	HRESULT RenderCamera();

public:
	static CAnimationNotify* Create(void* pArg = nullptr);
	virtual void Free() override;
};

END