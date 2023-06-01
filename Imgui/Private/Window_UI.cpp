#include "Window_UI.h"

CWindow_UI::CWindow_UI()
{
}

HRESULT CWindow_UI::Initialize(void* pArg)
{
	m_vWindowPos = ImVec2(0, 200);
	m_vWindowSize = ImVec2(500, 400);
	return S_OK;
}

void CWindow_UI::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);
	Begin("UI", nullptr, m_WindowFlag);


	if (Button("Open File Dialog"))
		IMFILE->OpenDialog("ChooseFileDlgKey", "Choose File", ".png, .dds", ".");
	
	// display
	if (IMFILE->Display("ChooseFileDlgKey"))
	{
		// action if OK
		if (IMFILE->IsOk())
		{
			string filePathName = IMFILE->GetFilePathName();
			string filePath = IMFILE->GetCurrentPath();

			wstring fPath;
			fPath.assign(filePathName.begin(), filePathName.end());
			const _tchar* test = fPath.c_str();
		}

		// close
		IMFILE->Close();
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
