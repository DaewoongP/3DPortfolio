#pragma once
#include "Model.h"

BEGIN(Engine)

class CChannel final : public CBase
{
private:
	explicit CChannel();
	virtual ~CChannel() = default;

public:
	_uint Get_BoneIndex() const { return m_iBoneIndex; }
	// 존재하는 키프레임 중 인덱스에 해당하는 키프레임의 시간값 반환
	_double Get_CurrentKeyFrameTime(_uint iCurrentIndex) const { return m_KeyFrames[iCurrentIndex].dTime; }
	// 채널 키프레임 중 가장 큰값 반환
	_uint	Get_NumKeyFrames() const { return m_iNumKeyFrames; }

public:
	HRESULT Initialize(const Engine::CHANNEL& Channel, const CModel::BONES& Bones);
	// Time Acc에 해당하는 현재 채널의 키프레임 상태값을 기반으로 선형보간하여 뼈에 상태행렬을 전달하는 함수.
	void	Invalidate_TransformationMatrix(CModel::BONES& Bones, _double dTimeAcc, _Inout_ _uint* pCurrentKeyFrameIndex);
	void	Lerp_TransformationMatrix(CModel::BONES& Bones, class CChannel* pCurrentChannel, _double dTimeAcc, _uint iCurrentKeyFrameIndex);

private:
	_tchar				m_szName[MAX_STR] = TEXT("");
	// 스케일, 로테이션, 포지션 중 가장 프레임이 많은 값을 저장.
	_uint				m_iNumKeyFrames = { 0 };
	// 인덱스에 해당하는 키프레임에서의 상태값들 (SRT)을 벡터컨테이너에 저장
	// 배열 인덱스로 바로 접근하여 인덱스에 해당하는 키프레임 반환 가능.
	vector<KEYFRAME>	m_KeyFrames;
	// 현재 채널과 이름이 같은 뼈의 인덱스 값
	_uint				m_iBoneIndex = { 0 };

public:
	static CChannel* Create(const Engine::CHANNEL& Channel, const CModel::BONES& Bones);
	virtual void Free() override;
};

END