// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
// Windows 头文件: 
#include <windows.h>

// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <vector>
#include <algorithm>
#include <random>



#ifdef _DEBUG
//#define TEST
#define ASSERT(exp)	\
	if (!(exp))		\
		__asm { int 3}
#else
#define ASSERT(exp)	0
#endif

#define CHECK_ASSERT_RET_FALSE(exp)	\
	ASSERT(exp);					\
	if (!exp) return false;

#define CHECK_RET_FALSE(exp)	\
	if (!exp) return false;


#define FIVE_IN_A_ROW

#ifdef FIVE_IN_A_ROW
#define CHESS_TYPE bool
#define CHESS_BLACK true
#define CHESS_WHITE false

#define FIVE 5
#define UNUSEFULL 0xffff
#endif

struct CHESS_POINT
{
	UINT16 row;
	UINT16 col;
	CHESS_POINT(const UINT16 & _row, const UINT16 & _col) : row(_row), col(_col){}
};

// TODO: 在此处引用程序需要的其他头文件
