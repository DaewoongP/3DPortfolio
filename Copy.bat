
xcopy				/y /s		.\Engine\Public\*.*					.\Reference\Headers\
attrib				+R												.\Reference\Headers\*.*
xcopy				/y /s		.\Engine\ThirdPartyLib\*.*			.\Reference\Librarys\
attrib				+R												.\Reference\Librarys\*.*

xcopy				/y			.\Engine\Bin\Engine.lib				.\Reference\Librarys\
xcopy				/y			.\Engine\Bin\Engine.dll				.\Client\Bin\
xcopy				/y			.\Engine\Bin\Engine.dll				.\Tool\
xcopy				/y			.\Engine\Bin\Engine.dll				.\Imgui\Bin\
xcopy				/y			.\Engine\Bin\Engine.dll				.\Assimp\Bin\

xcopy				/y			.\Engine\Bin\Engine_d.lib			.\Reference\Librarys\
xcopy				/y			.\Engine\Bin\Engine_d.dll			.\Client\Bin\
xcopy				/y			.\Engine\Bin\Engine_d.dll			.\Tool\
xcopy				/y			.\Engine\Bin\Engine_d.dll			.\Imgui\Bin\
xcopy				/y			.\Engine\Bin\Engine_d.dll			.\Assimp\Bin\

xcopy				/y			.\Engine\Bin\ShaderFiles\*.hlsl		.\Client\Bin\ShaderFiles\
attrib				+R												.\Client\Bin\ShaderFiles\*.*
xcopy				/y			.\Engine\Bin\ShaderFiles\*.hlsl		.\Imgui\Bin\ShaderFiles\
attrib				+R												.\Imgui\Bin\ShaderFiles\*.*