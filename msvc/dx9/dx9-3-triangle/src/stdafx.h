#pragma once

#ifndef UNICODE
#error Required UNICODE
#endif

#include <locale.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <wchar.h>
#include <stdio.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <timeapi.h>
#include "d3d9.h"
#include "d3d9types.h"
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "d3d9.lib")


#ifndef container_of
#	if defined(_MSC_VER) || !defined(__llvm__)
#		define container_of CONTAINING_RECORD
#	else
#		define container_of(ptr, type, member) ({			\
			const __typeof__(((type *)0)->member) * __mptr = (ptr);	\
			(type *)((char *)ptr - offsetof(type, member)); })
#	endif
#endif
