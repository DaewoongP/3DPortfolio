#pragma once

namespace Engine
{
	enum MOUSEKEYSTATE { DIM_LB, DIM_RB, DIM_MB, DIM_END };
	enum MOUSEMOVESTATE { DIMS_X, DIMS_Y, DIMS_Z, DIMS_END };

	enum COMPONENTFLAG
	{
		// 16bit flag
		COM_VIBUFFER	= 0x0001,
		COM_TRANSFORM	= 0x0002,
		COM_COLLIDER	= 0x0004,
		COM_RIGIDBODY	= 0x0008,
		TEST_0			= 0x0010,
		TEST_1			= 0x0020,
		TEST_2			= 0x0040,
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
