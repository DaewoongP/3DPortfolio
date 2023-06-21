#pragma once
#include "../Public/ModelConverter.h"

BEGIN(Converter)

class CMainConverter final : public CBase
{
private:
	explicit CMainConverter();
	virtual ~CMainConverter() = default;

public:
	// 최초 구동 함수
	HRESULT Convert();

private:
	// 디렉토리를 순회하면서 모델파일을 바이너리화 하며 타입 설정.
	// 1. eType : 애니메이션모델, 스태틱모델 타입 설정
	// 2. DirectoryPath : 순회할 최상위폴더 설정.
	HRESULT ReadFileInDirectory(CModelConverter::TYPE eType, const _tchar* pDirectoryPath);

public:
	// 메인 클래스에서 실행할 함수
	static void Start();
	virtual void Free() override;
};

END