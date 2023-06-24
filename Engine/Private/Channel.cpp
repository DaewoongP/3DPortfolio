#include "..\Public\Channel.h"
#include "Bone.h"

CChannel::CChannel()
{
}

void CChannel::Set_CameraValueInFrame(_uint iFrameIndex, _float3 vEye, _float3 vAt)
{
	// ���ϴ� ī�޶��� ��ġ�� �ش� �����ӿ� ����.
	m_KeyFrames[iFrameIndex].vEye = vEye;
	m_KeyFrames[iFrameIndex].vAt = vAt;
}

HRESULT CChannel::Initialize(const Engine::CHANNEL& Channel, const CModel::BONES& Bones)
{
	lstrcpy(m_szName, Channel.Name);
	// ä�ΰ� �̸��� ���� ���� �ε����� ã�� ��������� ����.
	auto iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pValue) {
		if (!lstrcmp(m_szName, pValue->Get_Name()))
			return true;
		else
		{
			++m_iBoneIndex;
			return false;
		}
	});
	
	// SRT �� ���� ������������ ���� Ű�������� ã�� ����.
	// ��������� �������� �������� ���� ������ ������ ���������� �������� �������� ���������� �װ��� �����ϸ� �ǹǷ�. ���� ū ���� �����Ѵ�.
	m_iNumKeyFrames = max(Channel.NumScalingKeys, Channel.NumRotationKeys);
	m_iNumKeyFrames = max(m_iNumKeyFrames, Channel.NumPositionKeys);

	_float3	vScale;
	ZEROMEM(&vScale);
	_float4	vRotation;
	ZEROMEM(&vRotation);
	_float3	vTranslation;
	ZEROMEM(&vTranslation);

	// SRT�� ���� ���� �������� ���� ���� ��ȸ�ϸ� �� �����ӿ����� ���°��� ����.
	// ���� �ش� �����ӿ��� ���� ���� ��� ���������� ó���� ������ ����.
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

	// ������ Ű������ �ϰ�� ���º�ȯ�� �״�� ����.
	if (dTimeAcc >= LastKeyFrame.dTime)
	{
		vScale = LastKeyFrame.vScale;
		vRotation = LastKeyFrame.vRotation;
		vTranslation = LastKeyFrame.vTranslation;
	}
	else
	{
		// ���� TimeAcc ���� (Index ~ Index + 1) �ȿ� �����ϴ��� �˻�.
		// ���� �ȿ� �������� ������� �������� ���� ������ �˻�.
		while (dTimeAcc >= m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].dTime)
			++(*pCurrentKeyFrameIndex);

		// TimeAcc - Index.Time / (Index.Time ~ Index.Time + 1)
		_double		Ratio = (dTimeAcc - m_KeyFrames[(*pCurrentKeyFrameIndex)].dTime) /
			(m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].dTime - m_KeyFrames[(*pCurrentKeyFrameIndex)].dTime);

		// ���� �ε����� ���� �ε��� ���� ���º�ȯ���� ���������ϱ� ���� �� ����.
		_float3		vSourScale = m_KeyFrames[(*pCurrentKeyFrameIndex)].vScale;
		_float4		vSourRotation = m_KeyFrames[(*pCurrentKeyFrameIndex)].vRotation;
		_float3		vSourTranslation = m_KeyFrames[(*pCurrentKeyFrameIndex)].vTranslation;

		_float3		vDestScale = m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].vScale;
		_float4		vDestRotation = m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].vRotation;
		_float3		vDestTranslation = m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].vTranslation;

		// �������� �Լ�. Rotation�� ��� Quaternion ���¿��� Slerp �Լ� ���.
		XMStoreFloat3(&vScale, XMVectorLerp(XMLoadFloat3(&vSourScale), XMLoadFloat3(&vDestScale), (_float)Ratio));
		XMStoreFloat4(&vRotation, XMQuaternionSlerp(XMLoadFloat4(&vSourRotation), XMLoadFloat4(&vDestRotation), (_float)Ratio));
		XMStoreFloat3(&vTranslation, XMVectorLerp(XMLoadFloat3(&vSourTranslation), XMLoadFloat3(&vDestTranslation), (_float)Ratio));
	}

	// ���������Ǿ� ���۵� ���͸� ��Ŀ� ��� ä���� �̸��� ���� ���� �ε����� �� ����.
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
