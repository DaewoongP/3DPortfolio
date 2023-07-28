#include "..\Public\Light_Manager.h"

IMPLEMENT_SINGLETON(CLight_Manager)

CLight_Manager::CLight_Manager()
{

}

const CLight::LIGHTDESC * CLight_Manager::Get_Light(_uint iIndex)
{
	auto	iter = m_Lights.begin();

	for (size_t i = 0; i < iIndex; ++i)
		++iter;

	return (*iter)->Get_LightDesc();	
}

void CLight_Manager::Set_Light(_uint iIndex, CLight::LIGHTDESC LightDesc)
{
	auto	iter = m_Lights.begin();

	for (size_t i = 0; i < iIndex; ++i)
		++iter;

	(*iter)->Set_LightDesc(LightDesc);
}

CLight* CLight_Manager::Add_Lights(const CLight::LIGHTDESC & LightDesc)
{
	CLight*		pLight = CLight::Create(LightDesc);

	if (nullptr == pLight)
		return nullptr;

	m_Lights.push_back(pLight);

	return pLight;
}

HRESULT CLight_Manager::Clear_Lights()
{
	for (auto& pLight : m_Lights)
		Safe_Release(pLight);

	m_Lights.clear();

	return S_OK;
}

HRESULT CLight_Manager::Render_Lights(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	for (auto& pLight : m_Lights)
	{
		if (nullptr != pLight)
			pLight->Render(pShader, pVIBuffer);
	}

	return S_OK;
}

void CLight_Manager::Free()
{
	for (auto& pLight : m_Lights)
		Safe_Release(pLight);

	m_Lights.clear();
}
