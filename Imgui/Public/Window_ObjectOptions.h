#pragma once
#include "ImWindow.h"
#include "Dummy.h"
#include "Collider.h"
#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"

BEGIN(Tool)

class CWindow_ObjectOptions : public CImWindow
{
private:
	explicit CWindow_ObjectOptions(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CWindow_ObjectOptions() = default;

public:
	// 옵션을 바꿀 객체 세팅
	void Set_CurrentDummy(CDummy::DUMMYTYPE eDummyType, class CDummy* pDummy);

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double dTimeDelta) override;

private:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };

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
	CCollider::TYPE				m_ColliderType = { CCollider::TYPE_END };

	CBounding_Sphere::BOUNDINGSPHEREDESC	m_SphereDesc;
	CBounding_AABB::BOUNDINGAABBDESC		m_AABBDesc;
	CBounding_OBB::BOUNDINGOBBDESC			m_OBBDesc;

	CCollider*					m_pCollider = { nullptr };

private: /* For. Collider */
	HRESULT AddCollider();
	HRESULT SetUpSphere();
	HRESULT SetUpAABB();
	HRESULT SetUpOBB();

	HRESULT ColliderSaveLoad();
	HRESULT ColliderSaveButton();
	HRESULT ColliderWrite_File(const _tchar* pPath);
	HRESULT ColliderLoadButton();
	HRESULT ColliderRead_File(const _tchar* pFileName);

public:
	static CWindow_ObjectOptions* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
	virtual void Free() override;
};

END