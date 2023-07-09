#pragma once
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CBlackBoard final : public CBase
{
private:
	explicit CBlackBoard();
	virtual ~CBlackBoard() = default;

public:
	HRESULT Initialize();
	HRESULT Add_Value(const _tchar* pValueTag, void* pValue);
	void* Find_Value(const _tchar* pValueTag);

private:
	unordered_map<const _tchar*, void*>	m_Values;

public:
	static CBlackBoard* Create();
	virtual void Free() override;
};

END