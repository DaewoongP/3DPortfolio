
xcopy				/y /s		.\Engine\Public\*.*					.\Reference\Headers\
xcopy				/y /s		.\Engine9\Public\*.*				.\Reference\Headers9\

xcopy				/y			.\Engine\Bin\Engine.lib				.\Reference\Librarys\
xcopy				/y			.\Engine9\Bin\Engine9.lib			.\Reference\Librarys\
xcopy				/y			.\Engine\Bin\Engine.dll				.\Client\Bin\
xcopy				/y			.\Engine9\Bin\Engine9.dll			.\Tool\
