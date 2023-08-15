#pragma once

#define			BEGIN_INSTANCE		CGameInstance* pGameInstance = CGameInstance::GetInstance();\
									Safe_AddRef(pGameInstance);
#define			INSTANCE			pGameInstance
#define			END_INSTANCE		Safe_Release(pGameInstance);