#pragma once

#ifndef UNICODE
#error Required UNICODE
#endif

#include <locale.h>
#include <stdint.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <wchar.h>
#include <stdio.h>

// windows
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#include <commctrl.h>
#include <windowsx.h>

#include <dwmapi.h>
#pragma comment(lib,"dwmapi.lib")

#include <vssym32.h>
#include <uxtheme.h>
#pragma comment(lib, "uxtheme.lib")

// macros
#ifndef container_of
#	if defined(_MSC_VER) || !defined(__llvm__)
#		define container_of CONTAINING_RECORD
#	else
#		define container_of(ptr, type, member) ({ \
			const __typeof__(((type *)0)->member) * __mptr = (ptr);	\
			(type *)((char *)ptr - offsetof(type, member)); })
#	endif
#endif

#ifdef __cplusplus
#	define C_FUNCTION_BEGIN extern "C" {
#	define C_FUNCTION_END	};
#else
#	define C_FUNCTION_BEGIN
#	define C_FUNCTION_END
#endif
