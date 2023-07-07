#pragma once

#pragma warning (disable : 4005) // macro redefinition
#pragma warning (disable : 4251) // __declspec(dllexport)
#pragma warning (disable : 26812) // enum class
#pragma warning (disable : 4100) // unused variable
#pragma warning (disable : 6031) // except return
#pragma warning (disable : 4828) // korean comment
#pragma warning (disable : 26495) // Value Initialize

#define DIRECTINPUT_VERSION		0x0800
#include <dinput.h>

#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <DirectXCollision.h>

#include "fx11\d3dx11effect.h"
#include "DirectXTK\DDSTextureLoader.h"
#include "DirectXTK\WICTextureLoader.h"
#include "DirectXTK\ScreenGrab.h"
#include "DirectXTK\PrimitiveBatch.h"
#include "DirectXTK\VertexTypes.h"
#include "DirectXTK\Effects.h"
#include "DirectXTK\SpriteBatch.h"
#include "DirectXTK\SpriteFont.h"

#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <array>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <any>

// file system - c++ 17
#include <filesystem>
namespace fs = std::filesystem;

using namespace std;
using namespace DirectX;

#include "Engine_Enums.h"
#include "Engine_Function.h"
#include "Engine_TypeDef.h"
#include "Engine_Macro.h"
#include "Engine_Struct.h"

#include "Engine_Converter.h"

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

#endif // DBG_NEW
#endif // _DEBUG

#ifndef _DEBUG

#define New new

#endif // _DEBUG

using namespace Engine;
