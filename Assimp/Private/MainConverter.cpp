#include "..\Public\MainConverter.h"

CMainConverter::CMainConverter()
{
}

HRESULT CMainConverter::Convert()
{
	ReadFileInDirectory(CModelConverter::TYPE_ANIM, TEXT("C:\\Users\\msi\\Desktop\\GhostRunner\\Animations\\Game\\Visual\\Characters\\Hero_Swords\\Parts"));
	//ReadFileInDirectory(CModelConverter::TYPE_NONANIM, TEXT("C:\\Users\\msi\\Desktop\\GhostRunner\\Game"));
	return S_OK;
}

HRESULT CMainConverter::ReadFileInDirectory(CModelConverter::TYPE eType, const _tchar* pDirectoryPath)
{
	// 디렉토리 경로를 순회할 iterator
	fs::directory_iterator iter(fs::absolute(pDirectoryPath));
	
	while (iter != fs::end(iter))
	{
		// 실제 디렉토리 경로를 담고있는 변수 (iterator의 원본)
		const fs::directory_entry& entry = *iter;
		
		// 현재 entry 변수가 디렉토리인지 확인 후 디렉토리이면 재귀
		if (fs::is_directory(entry.path()))
		{
			ReadFileInDirectory(eType, entry.path().c_str());
		}
		else
		{
			// fbx파일 체크
			if (!lstrcmp(entry.path().extension().c_str(), TEXT(".fbx")) ||
				!lstrcmp(entry.path().extension().c_str(), TEXT(".FBX")))
			{
				std::cout << entry.path() << std::endl;
				
				// 해당하는 파일의 경로를 실제 모델 컨버터에 할당
				Converter::CModelConverter::Convert(eType, entry.path().string().c_str());
			}
		}

		iter++;
	}

	return S_OK;
}

void CMainConverter::Start()
{
	CMainConverter* pInstance = new CMainConverter();

	if (FAILED(pInstance->Convert()))
	{
		MSG_BOX("Convert Failed");
		Safe_Release(pInstance);
	}
	else
	{
		MSG_BOX("Convert Success!");
		Safe_Release(pInstance);
	}
}

void CMainConverter::Free()
{
}
