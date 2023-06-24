#include "..\Public\Channel.h"
#include "Bone.h"

CChannel::CChannel()
{
}

void CChannel::Set_CameraValueInFrame(_uint iFrameIndex, _float3 vEye, _float3 vAt)
{
	// 원하는 카메라의 위치를 해당 프레임에 저장.
	m_KeyFrames[iFrameIndex].vEye = vEye;
	m_KeyFrames[iFrameIndex].vAt = vAt;
}

HRESULT CChannel::Initialize(const Engine::CHANNEL& Channel, const CModel::BONES& Bones)
{
	lstrcpy(m_szName, Channel.Name);
	// 채널과 이름이 같은 뼈의 인덱스를 찾아 멤버변수에 저장.
	auto iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pValue) {
		if (!lstrcmp(m_szName, pValue->Get_Name()))
			return true;
		else
		{
			++m_iBoneIndex;
			return false;
		}
	});
	
	// SRT 중 가장 많은프레임을 가진 키프레임을 찾아 저장.
	// 상대적으로 적은수의 프레임을 가진 변수는 어차피 마지막값을 기준으로 프레임이 끝날때까지 그값을 유지하면 되므로. 가장 큰 값만 저장한다.
	m_iNumKeyFrames = max(Channel.NumScalingKeys, Channel.NumRotationKeys);
	m_iNumKeyFrames = max(m_iNumKeyFrames, Channel.NumPositionKeys);

	_float3	vScale;
	ZEROMEM(&vScale);
	_float4	vRotation;
	ZEROMEM(&vRotation);
	_float3	vTranslation;
	ZEROMEM(&vTranslation);

	// SRT중 가장 많은 프레임을 가진 값을 순회하며 각 프레임에서의 상태값을 저장.
	// 만약 해당 프레임에서 값이 없을 경우 마지막으로 처리한 값으로 저장.
	for (_uint i = 0; i < m_iNumKeyFrames; ++i)
	{
		KEYFRAME				Keyframe;

		if (Channel.NumScalingKeys > i)
		{
			vScale = Channel.ScalingKeys[i].Value;
			Keyframe.dTime = Channel.ScalingKeys[i].Time;
		}

		if (Channel.NumRotationKeys > i)
		{
			vRotation = Channel.RotationKeys[i].Value;
			Keyframe.dTime = Channel.RotationKeys[i].Time;
		}

		if (Channel.NumPositionKeys > i)
		{
			vTranslation = Channel.PositionKeys[i].Value;
			Keyframe.dTime = Channel.PositionKeys[i].Time;
		}

		Keyframe.vScale = vScale;
		Keyframe.vRotation = vRotation;
		Keyframe.vTranslation = vTranslation;

		m_KeyFrames.push_back(Keyframe);
	}

	return S_OK;
}

void CChannel::Invalidate_TransformationMatrix(CModel::BONES& Bones, _double dTimeAcc, _Inout_ _uint* pCurrentKeyFrameIndex)
{
	if (0.0 == dTimeAcc)
		*pCurrentKeyFrameIndex = 0;

	KEYFRAME LastKeyFrame = m_KeyFrames.back();

	_float3	vScale;
	ZEROMEM(&vScale);
	_float4	vRotation;
	ZEROMEM(&vRotation);
	_float3	vTranslation;
	ZEROMEM(&vTranslation);

	// 마지막 키프레임 일경우 상태변환을 그대로 유지.
	if (dTimeAcc >= LastKeyFrame.dTime)
	{
		vScale = LastKeyFrame.vScale;
		vRotation = LastKeyFrame.vRotation;
		vTranslation = LastKeyFrame.vTranslation;
	}
	else
	{
		// 현재 TimeAcc 값이 (Index ~ Index + 1) 안에 존재하는지 검사.
		// 만약 안에 존재하지 않을경우 증가시켜 다음 프레임 검사.
		while (dTimeAcc >= m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].dTime)
			++(*pCurrentKeyFrameIndex);

		// TimeAcc - Index.Time / (Index.Time ~ Index.Time + 1)
		_double		Ratio = (dTimeAcc - m_KeyFrames[(*pCurrentKeyFrameIndex)].dTime) /
			(m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].dTime - m_KeyFrames[(*pCurrentKeyFrameIndex)].dTime);

		// 현재 인덱스와 다음 인덱스 까지 상태변환들을 선형보간하기 위한 값 저장.
		_float3		vSourScale = m_KeyFrames[(*pCurrentKeyFrameIndex)].vScale;
		_float4		vSourRotation = m_KeyFrames[(*pCurrentKeyFrameIndex)].vRotation;
		_float3		vSourTranslation = m_KeyFrames[(*pCurrentKeyFrameIndex)].vTranslation;

		_float3		vDestScale = m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].vScale;
		_float4		vDestRotation = m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].vRotation;
		_float3		vDestTranslation = m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].vTranslation;

		// 선형보간 함수. Rotation의 경우 Quaternion 형태여서 Slerp 함수 사용.
		XMStoreFloat3(&vScale, XMVectorLerp(XMLoadFloat3(&vSourScale), XMLoadFloat3(&vDestScale), (_float)Ratio));
		XMStoreFloat4(&vRotation, XMQuaternionSlerp(XMLoadFloat4(&vSourRotation), XMLoadFloat4(&vDestRotation), (_float)Ratio));
		XMStoreFloat3(&vTranslation, XMVectorLerp(XMLoadFloat3(&vSourTranslation), XMLoadFloat3(&vDestTranslation), (_float)Ratio));
	}

	// 선형보간되어 제작된 벡터를 행렬에 담아 채널의 이름과 같은 뼈의 인덱스에 값 전달.
	_matrix		TransformationMatrix = XMMatrixAffineTransformation(XMLoadFloat3(&vScale), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMLoadFloat4(&vRotation), XMLoadFloat3(&vTranslation));
	Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
}

CChannel* CChannel::Create(const Engine::CHANNEL& Channel, const CModel::BONES& Bones)
{
	CChannel* pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(Channel, Bones)))
	{
		MSG_BOX("Failed to Created CChannel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CChannel::Free()
{
}
