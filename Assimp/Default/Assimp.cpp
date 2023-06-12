#include "MainConverter.h"

int main()
{
#ifdef _DEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif // _DEBUG

    CMainConverter* pMainConverter = CMainConverter::Create();

    if (FAILED(pMainConverter->Convert()))
    {
        MSG_BOX("Convert Failed");
    }
    else
    {
        MSG_BOX("Convert Success");
    }

    Safe_Release(pMainConverter);
}
