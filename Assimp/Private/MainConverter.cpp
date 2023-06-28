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
	// ���丮 ��θ� ��ȸ�� iterator
	fs::directory_iterator iter(fs::absolute(pDirectoryPath));
	
	while (iter != fs::end(iter))
	{
		// ���� ���丮 ��θ� ����ִ� ���� (iterator�� ����)
		const fs::directory_entry& entry = *iter;
		
		// ���� entry ������ ���丮���� Ȯ�� �� ���丮�̸� ���
		if (fs::is_directory(entry.path()))
		{
			ReadFileInDirectory(eType, entry.path().c_str());
		}
		else
		{
			// fbx���� üũ
			if (!lstrcmp(entry.path().extension().c_str(), TEXT(".fbx")) ||
				!lstrcmp(entry.path().extension().c_str(), TEXT(".FBX")))
			{
				std::cout << entry.path() << std::endl;
				
				// �ش��ϴ� ������ ��θ� ���� �� �����Ϳ� �Ҵ�
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
