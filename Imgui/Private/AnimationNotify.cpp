#include "..\Public\AnimationNotify.h"

CAnimationNotify::CAnimationNotify()
{
}

HRESULT CAnimationNotify::Initialize(void* pArg)
{
	return S_OK;
}

void CAnimationNotify::Tick(_double dTimeDelta)
{
	ImGui::Begin("Animation Notify");
	// 사운드, 파티클, 이펙트, 카메라 등등의 처리
	if (ImGui::BeginTabBar("Notify"))
	{
		if (ImGui::BeginTabItem("Camera"))
		{
			NotifyCamera();

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();
}

HRESULT CAnimationNotify::NotifyCamera()
{
	return S_OK;
}

CAnimationNotify* CAnimationNotify::Create(void* pArg)
{
	CAnimationNotify* pInstance = New CAnimationNotify();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created CAnimationNotify");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnimationNotify::Free()
{
}
