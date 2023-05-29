#pragma once

class CFileInfo
{
private:
	CFileInfo() = default;
	~CFileInfo() = default;

	typedef	struct tagTexturePath
	{
		wstring			wstrObjKey = L"";
		wstring			wstrStateKey = L"";
		wstring			wstrPath = L"";
		int				iCount = 0;

	}IMGPATH;
public:
	static CString		ConvertRelativePath(CString strFullPath);
	static void			DirInfoExtraction(const wstring& wstrPath, list<IMGPATH*>& rPathInfoList);
	static int			DirFileCnt(const wstring& wstrPath);
	static int			DirCnt(const wstring & wstrPath);
	static int			ExtractIntOfBackFromCString(CString& cstr);
};