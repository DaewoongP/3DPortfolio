
xcopy				/y /s		.\Engine\Public\*.*					.\Reference\Headers\
xcopy				/y /s		.\Engine\ThirdPartyLib\*.*			.\Reference\Librarys\

xcopy				/y			.\Engine\Bin\Engine.lib				.\Reference\Librarys\
xcopy				/y			.\Engine\Bin\Engine.dll				.\Client\Bin\
xcopy				/y			.\Engine\Bin\Engine.dll				.\Tool\
xcopy				/y			.\Engine\Bin\Engine.dll				.\Imgui\Bin\
xcopy				/y			.\Engine\Bin\Engine.dll				.\Assimp\Bin\

xcopy				/y			.\Engine\Bin\Engine_d.lib				.\Reference\Librarys\
xcopy				/y			.\Engine\Bin\Engine_d.dll				.\Client\Bin\
xcopy				/y			.\Engine\Bin\Engine_d.dll				.\Tool\
xcopy				/y			.\Engine\Bin\Engine_d.dll				.\Imgui\Bin\
xcopy				/y			.\Engine\Bin\Engine_d.dll				.\Assimp\Bin\