#pragma once

#pragma warning (disable : 4005)
#pragma warning (disable : 4251)
#pragma warning (disable : 26812)
#pragma warning (disable : 4828)
#pragma warning (disable : 4702)

#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>

#include "fx11\d3dx11effect.h"
#include "DirectXTK\DDSTextureLoader.h"
#include "DirectXTK\WICTextureLoader.h"

#define DIRECTINPUT_VERSION		0x0800
#include <dinput.h>

#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <string>

using namespace std;
using namespace DirectX;

#include "Engine_Enums.h"
#include "Engine_Function.h"
#include "Engine_TypeDef.h"
#include "Engine_Macro.h"
#include "Engine_Struct.h"

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 

#ifndef _USE_IMGUI
#define new DBG_NEW 
#else
#define New DBG_NEW
#endif // _USE_IMGUI

#endif // _DEBUG
#endif // _DEBUG

using namespace Engine;
