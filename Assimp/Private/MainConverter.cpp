#include "..\Public\MainConverter.h"

CMainConverter::CMainConverter()
{
}

HRESULT CMainConverter::Initialize()
{
	m_pModel = Converter::CModel::Create(CModel::TYPE_ANIM, "../Bin/Resources/Hand/Hand.fbx");
	return S_OK;
}

HRESULT CMainConverter::Convert()
{
	return S_OK;
}

CMainConverter* CMainConverter::Create()
{
	CMainConverter* pInstance = new CMainConverter();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created CMainConverter");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMainConverter::Free()
{
	Safe_Release(m_pModel);
}
