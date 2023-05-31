#include "Window_UI.h"

CWindow_UI::CWindow_UI()
{
}

HRESULT CWindow_UI::Initialize(void* pArg)
{
	m_vWindowPos = ImVec2(0, 100);
	m_vWindowSize = ImVec2(500, 500);
	return S_OK;
}

void CWindow_UI::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);
	Begin("UI", nullptr, m_WindowFlag);
	if (BeginListBox("listbox 1"))
	{
		if (TreeNode("Combo 1"))
		{
			if (TreeNode("Combo 2"))
			{
				if (TreeNode("Combo 3"))
				{


					TreePop();
				}

				TreePop();
			}

			if (TreeNode("Combo 4"))
			{


				TreePop();
			}
			TreePop();
		}

		EndListBox();
	}

	End();
}

CWindow_UI* CWindow_UI::Create(void* pArg)
{
	CWindow_UI* pInstance = new CWindow_UI();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created CWindow_UI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CWindow_UI::Free()
{
	__super::Free();
}
