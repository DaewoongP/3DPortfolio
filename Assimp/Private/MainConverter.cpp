#include "..\Public\MainConverter.h"

CMainConverter::CMainConverter()
{
}

HRESULT CMainConverter::Convert()
{
	ReadFileInDirectory(CModel::TYPE_ANIM, TEXT("..\\..\\Resources\\Model\\Anim"));
	ReadFileInDirectory(CModel::TYPE_NONANIM, TEXT("..\\..\\Resources\\Model\\NonAnim"));
	return S_OK;
}

HRESULT CMainConverter::ReadFileInDirectory(CModel::TYPE eType, const _tchar* pDirectoryPath)
{
	// current_path == project path
	// () operator  setting path
	
	fs::directory_iterator iter(fs::absolute(pDirectoryPath));
	
	while (iter != fs::end(iter))
	{
		// directory path variable
		// ex) c/~/~/MyDirectory
		const fs::directory_entry& entry = *iter;
		
		// Check Directory
		if (fs::is_directory(entry.path()))
		{
			ReadFileInDirectory(eType, entry.path().c_str());
		}
		else
		{
			// extension find
			if (!lstrcmp(entry.path().extension().c_str(), TEXT(".fbx")) ||
				!lstrcmp(entry.path().extension().c_str(), TEXT(".FBX")))
			{
				// extension file store
				Converter::CModel::Convert(eType, entry.path().string().c_str());

				std::cout << entry.path() << std::endl;
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
