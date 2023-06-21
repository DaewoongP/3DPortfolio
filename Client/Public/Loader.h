#pragma once
#include "Base.h"
#include "Client_Defines.h"

#include "Model.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CLoader final : public CBase
{
private:
	explicit CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLoader() = default;

public:
	const _tchar* Get_LoadingText() const { return m_szLoading; }
	_bool Get_Finished() const { return m_isFinished; }

public:
	HRESULT Initialize(LEVELID eNextLevel);
	HRESULT Loading();

private:
	// 쓰레드 생성 핸들
	HANDLE					m_hThread = { 0 };
	// 다중 쓰레드에서 해당 쓰레드가 크리티컬 섹션을 벗어나기 전까지 다른 쓰레드가 실행되지 못하게 처리 하는 변수.
	CRITICAL_SECTION		m_Critical_Section;

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };
	CGameInstance*			m_pGameInstance = { nullptr };

private:
	LEVELID					m_eNextLevelID = { LEVELID::LEVEL_END };
	// 로딩 상태 확인용 문자열 변수
	_tchar					m_szLoading[MAX_STR] = TEXT("");
	// 로딩이 끝났는지 확인하는 변수
	_bool					m_isFinished = { false };

private:
	HRESULT Loading_For_Logo();
	HRESULT Loading_For_GamePlay();
	// 파일시스템에서 데이터 파일을 한번에 프로토타입으로 만들기 위한 함수.
	HRESULT Ready_Prototype_Component_ModelData(CModel::TYPE eType, const _tchar* pPath, const _tchar* pPrototypeTag);

public:
	static CLoader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevel);
	virtual void Free() override;
};

END