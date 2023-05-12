#include "pch.h"
#include "FileInfo.h"

CString CFileInfo::ConvertRelativePath(CString strFullPath)
{
	TCHAR	szRelativePath[MAX_PATH] = L"";
	TCHAR	szCurDirPath[MAX_PATH] = L"";

	GetCurrentDirectory(MAX_PATH, szCurDirPath);

	// PathRelativePathTo : szCurDirPath에서 strFullPath로 가는 상대 경로를 만들어 szRelativePath에 저장해주는 함수
	// 단, 같은 드라이브 내에서만 경로 변경 가능

	PathRelativePathTo(szRelativePath,
		szCurDirPath,
		FILE_ATTRIBUTE_DIRECTORY,
		strFullPath.GetString(),
		FILE_ATTRIBUTE_DIRECTORY);

	return CString(szRelativePath);
}

void CFileInfo::DirInfoExtraction(const wstring & wstrPath, list<IMGPATH*>& rPathInfoList)
{
	//CFileFind : MFC에서 제공하는 파일 및 경로 제어 관련 클래스
	CFileFind			Find;

	// 경로 상의 모든 파일을 찾아야 하기 때문에 경로 수정
	wstring		wstrFilePath = wstrPath + L"\\*.*";

	// FindFile : 해당 경로에 파일의 유무를 확인해주는 함수
	// 파일이 있으면 true, 없으면 false

	// 파일에 접근하고 싶을 경우, FindNextFile 함수를 무조건 호출해야 한다.
	// FindNextFile 함수를 호출하면 파일들 중 가장 첫 번째 파일에 접근이 가능

	BOOL		bContinue = Find.FindFile(wstrFilePath.c_str());

	while (bContinue)
	{
		bContinue = Find.FindNextFile();

		if (Find.IsDots())
			continue;

		else if (Find.IsDirectory())
		{
			// GetFilePath : 현재 찾은 경로를 얻어오는 함수
			DirInfoExtraction(wstring(Find.GetFilePath()), rPathInfoList);
		}
		else
		{
			if (Find.IsSystem())
				continue;

			IMGPATH*		pImgPath = new IMGPATH;
			TCHAR			szPathBuf[MAX_STR] = L"";

			lstrcpy(szPathBuf, Find.GetFilePath().GetString());

			// L"D:\\유준환\\135기\\Frame135\\Texture\\Stage\\Effect\\BossMultiAttack\\BossMultiAttack0.png"

			PathRemoveFileSpec(szPathBuf);

			//L"D:\\유준환\\135기\\Frame135\\Texture\\Stage\\Effect\\BossMultiAttack"

			pImgPath->iCount = DirFileCnt(szPathBuf);

			// GetFileTitle : 파일 이름만 얻어옴
			wstring		wstrTextureName = Find.GetFileTitle().GetString();
			// wstrTextureName = L"BossMultiAttack0"

			// substr(시작, 끝) : 시작에서 끝에 해당하는 문자열을 얻어옴
			wstrTextureName = wstrTextureName.substr(0, wstrTextureName.size() - 1) + L"%d.png";

			// wstrTextureName = L"BossMultiAttack%d.png"

			//PathCombine : 2, 3 매개 변수를 결합하여 1매개 변수를 만들어줌
			PathCombine(szPathBuf, szPathBuf, wstrTextureName.c_str());
			// L"D:\\유준환\\135기\\Frame135\\Texture\\Stage\\Effect\\BossMultiAttack\\BossMultiAttack%d.png"

			pImgPath->wstrPath = ConvertRelativePath(szPathBuf);
			// ..\\Frame135\\Texture\\Stage\\Effect\\BossMultiAttack\\BossMultiAttack%d.png"

			PathRemoveFileSpec(szPathBuf);
			// ..\\Frame135\\Texture\\Stage\\Effect\\BossMultiAttack\\

			// PathFindFileName : 파일명을 찾거나 가장 말단의 폴더명을 찾아내는 함수
			pImgPath->wstrStateKey = PathFindFileName(szPathBuf);

			PathRemoveFileSpec(szPathBuf);
			// ..\\Frame135\\Texture\\Stage\\Effect\\

			pImgPath->wstrObjKey = PathFindFileName(szPathBuf);

			rPathInfoList.push_back(pImgPath);

			bContinue = false;
		}
	}
}

int CFileInfo::DirFileCnt(const wstring & wstrPath)
{
	wstring		wstrFilePath = wstrPath + L"\\*.*";

	CFileFind			Find;
	BOOL		bContinue = Find.FindFile(wstrFilePath.c_str());

	int	iFileCnt = 0;

	while (bContinue)
	{
		bContinue = Find.FindNextFile();

		if (Find.IsDots())
			continue;

		if (Find.IsSystem())
			continue;

		++iFileCnt;
	}


	return iFileCnt;
}

int CFileInfo::DirCnt(const wstring & wstrPath)
{
	wstring		wstrFilePath = wstrPath + L"\\*.*";

	CFileFind			Find;
	BOOL		bContinue = Find.FindFile(wstrFilePath.c_str());

	int	iDirCnt = 0;

	while (bContinue)
	{
		bContinue = Find.FindNextFile();

		if (Find.IsDots())
			continue;

		if (Find.IsSystem())
			continue;

		if (Find.IsDirectory())
			++iDirCnt;
	}

	return iDirCnt;
}

int CFileInfo::ExtractIntOfBackFromCString(CString & cstr)
{
	int i = 0;
	for (; i < cstr.GetLength(); ++i)
	{
		// isdigit : 매개 변수로 넘겨받은 문자가 글자형 문자인지 숫자형 문자인기 구분해주는 함수
		// 숫자 형 문자라 판단할 경우 0이 아닌 값을 반환
		if (0 != isdigit(cstr[i]))
			break;
	}
	cstr.Delete(0, i);
	
	return _tstoi(cstr);
}
