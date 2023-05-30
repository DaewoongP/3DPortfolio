#pragma once

namespace Engine
{
	

	enum COMPONENTFLAG
	{
		// 16bit flag
		COM_VIBUFFER	= 0x0001,
		COM_TRANSFORM	= 0x0002,
		COM_COLLIDER	= 0x0004,
		COM_RIGIDBODY	= 0x0008,
		COM_CAMERA		= 0x0010,
		COM_RENDERER	= 0x0020,
		COM_TEXTURE		= 0x0040,
		TEST_3			= 0x0080,
		TEST_4			= 0x0100,
		TEST_5			= 0x0200,
		TEST_6			= 0x0400,
		TEST_7			= 0x0800,
		TEST_8			= 0x1000,
		TEST_9			= 0x2000,
		TEST_10			= 0x4000,
		TEST_11			= 0x8000,
	};
}
