#pragma once
#include "assimp\Importer.hpp"
#include "assimp\scene.h"
#include "assimp\postprocess.h"

#include "Assimp_Struct.h"

// file system - c++ 17
#include <filesystem>
namespace fs = std::filesystem;

namespace Converter
{
	const unsigned int g_iWinSizeX = 1280;
	const unsigned int g_iWinSizeY = 720;
}

using namespace Converter;