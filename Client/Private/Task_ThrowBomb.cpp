#include "..\Public\Task_ThrowBomb.h"
#include "GameInstance.h"
#include "Boss.h"

HRESULT CTask_ThrowBomb::Initialize(CBlackBoard* pBlackBoard)
{
	if (FAILED(__super::Initialize(pBlackBoard)))
		return E_FAIL;

	m_pOwner = static_cast<CGameObject*>(m_pBlackBoard->Find_Value(TEXT("Value_Owner")));

	return S_OK;
}

CBehavior::STATE CTask_ThrowBomb::Tick(_double dTimeDelta)
{
	if (nullptr == m_pOwner)
		return STATE_FAILED;

	CGameObject* pTarget = *reinterpret_cast<CGameObject**>(m_pBlackBoard->Find_Value(TEXT("Value_Target")));

	if (nullptr == pTarget)
		return STATE_FAILED;

	_vector vTargetPos = pTarget->Get_Transform()->Get_State(CTransform::STATE_POSITION);
	vTargetPos = XMVectorSetY(vTargetPos, 0.f);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	for (_uint i = 0; i < 7; ++i)
	{
		pGameInstance->Play_Sound(TEXT("Boss_ThrowBomb%d.ogg"), 4, CSound_Manager::SOUND_BOSS, 0.3f, true);

		_vector vRand = XMVectorSet(((rand() % 181) - 90.f), 0.f, ((rand() % 181) - 90.f), 0.f);

		static_cast<CBoss*>(m_pOwner)->Throw_Bomb(vTargetPos + vRand);
	}

	Safe_Release(pGameInstance);
	
	return STATE_SUCCESS;
}

HRESULT CTask_ThrowBomb::Reset()
{
	if (FAILED(__super::Reset()))
		return E_FAIL;

	return S_OK;
}

CTask_ThrowBomb* CTask_ThrowBomb::Create(CBlackBoard* pBlackBoard)
{
	CTask_ThrowBomb* pInstance = new CTask_ThrowBomb();

	if (FAILED(pInstance->Initialize(pBlackBoard)))
	{
		MSG_BOX("Failed to Created CTask_ThrowBomb");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTask_ThrowBomb::Free()
{
	__super::Free();
}
