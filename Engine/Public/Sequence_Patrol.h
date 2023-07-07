#pragma once
#include "Sequence.h"

BEGIN(Engine)

class ENGINE_DLL CSequence_Patrol final : public CSequence
{
private:
	explicit CSequence_Patrol() = default;
	virtual ~CSequence_Patrol() = default;

public:
	virtual HRESULT Initialize(CBlackBoard* pBlackBoard) override;

public:
	static CSequence_Patrol* Create(CBlackBoard* pBlackBoard);
	virtual void Free() override;
};

END