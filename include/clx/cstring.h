/* ------------------------------------------------------------------------- */
/*
 *  cstring.h
 *
 *  Copyright (c) 2004 - 2010, clown. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    - Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    - Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    - No names of its contributors may be used to endorse or promote
 *      products derived from this software without specific prior written
 *      permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 *  TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  Last-modified: Fri 30 Jul 2010 22:38:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_CSTRING_H
#define CLX_CSTRING_H

#include "config.h"
#include <ctime>
#include <cstring>
#include <cstdlib>
#ifdef CLX_USE_WCHAR
#include <cwchar>
#endif // CLX_USE_WCHAR

namespace clx {
	using std::strlen;
	using std::strcmp;
	using std::strncmp;
	using std::strcoll;
	using std::strcpy;
	using std::strncpy;
	using std::strcat;
	using std::strncat;
	using std::strchr;
	using std::strrchr;
	using std::strstr;
	using std::strspn;
	using std::strcspn;
	using std::strpbrk;
	using std::strtol;
	using std::strtoul;
	using std::strtod;
	using std::strftime;
	using std::strxfrm;
	
#ifdef CLX_USE_WCHAR
	/* --------------------------------------------------------------------- */
	//  strlen
	/* --------------------------------------------------------------------- */
	inline size_t strlen(const wchar_t* str) {
		return std::wcslen(str);
	}
	
	/* --------------------------------------------------------------------- */
	//  strcmp
	/* --------------------------------------------------------------------- */
	inline int strcmp(const wchar_t* s1, const wchar_t* s2) {
		return std::wcscmp(s1, s2);
	}
	
	/* --------------------------------------------------------------------- */
	//  strncmp
	/* --------------------------------------------------------------------- */
	inline int strncmp(const wchar_t* s1, const wchar_t* s2, size_t n) {
		return std::wcsncmp(s1, s2, n);
	}
	
	/* --------------------------------------------------------------------- */
	//  strcoll
	/* --------------------------------------------------------------------- */
	inline int strcoll(const wchar_t* s1, const wchar_t* s2) {
		return std::wcscoll(s1, s2);
	}
	
	/* --------------------------------------------------------------------- */
	//  strcpy
	/* --------------------------------------------------------------------- */
	inline wchar_t* strcpy(wchar_t* dest, const wchar_t* src) {
		return std::wcscpy(dest, src);
	}
	
	/* --------------------------------------------------------------------- */
	//  strncpy
	/* --------------------------------------------------------------------- */
	inline wchar_t* strncpy(wchar_t* dest, const wchar_t* src, size_t n) {
		return std::wcsncpy(dest, src, n);
	}
	
	/* --------------------------------------------------------------------- */
	//  strcat
	/* --------------------------------------------------------------------- */
	inline wchar_t* strcat(wchar_t* dest, const wchar_t* src) {
		return std::wcscat(dest, src);
	}
	
	/* --------------------------------------------------------------------- */
	//  strncat
	/* --------------------------------------------------------------------- */
	inline wchar_t* strncat(wchar_t* dest, const wchar_t* src, size_t n) {
		return std::wcsncat(dest, src, n);
	}
	
	/* --------------------------------------------------------------------- */
	//  strchr
	/* --------------------------------------------------------------------- */
	inline const wchar_t* strchr(const wchar_t* str, wchar_t c) {
		return std::wcschr(str, c);
	}
	
	inline wchar_t* strchr(wchar_t* str, wchar_t c) {
		return std::wcschr(str, c);
	}
	
	/* --------------------------------------------------------------------- */
	//  strrchr
	/* --------------------------------------------------------------------- */
	inline const wchar_t* strrchr(const wchar_t* str, wchar_t c) {
		return std::wcsrchr(str, c);
	}
	
	inline wchar_t* strrchr(wchar_t* str, wchar_t c) {
		return std::wcsrchr(str, c);
	}
	
	/* --------------------------------------------------------------------- */
	//  strstr
	/* --------------------------------------------------------------------- */
	inline const wchar_t* strstr(const wchar_t* str, const wchar_t* substr) {
		return std::wcsstr(str, substr);
	}
	
	inline wchar_t* strstr(wchar_t* str, const wchar_t* substr) {
		return std::wcsstr(str, substr);
	}
	
	/* --------------------------------------------------------------------- */
	//  strspn
	/* --------------------------------------------------------------------- */
	inline size_t strspn(const wchar_t* str, const wchar_t* spanset) {
		return std::wcsspn(str, spanset);
	}
	
	/* --------------------------------------------------------------------- */
	//  strcspn
	/* --------------------------------------------------------------------- */
	inline size_t strcspn(const wchar_t* src, const wchar_t* spanset) {
		return std::wcscspn(src, spanset);
	}
	
	/* --------------------------------------------------------------------- */
	//  strpbrk
	/* --------------------------------------------------------------------- */
	inline const wchar_t* strpbrk(const wchar_t* str, const wchar_t* spanset) {
		return std::wcspbrk(str, spanset);
	}
	
	inline wchar_t* strpbrk(wchar_t* str, const wchar_t* spanset) {
		return std::wcspbrk(str, spanset);
	}
	
	/* --------------------------------------------------------------------- */
	//  strtol
	/* --------------------------------------------------------------------- */
	inline long int wcstol(const wchar_t* str, wchar_t** end, int base) {
		return std::wcstol(str, end, base);
	}
	
	/* --------------------------------------------------------------------- */
	//  strtoul
	/* --------------------------------------------------------------------- */
	inline unsigned long int strtoul(const wchar_t* str, wchar_t** end, int base) {
		return std::wcstoul(str, end, base);
	}
	
	/* --------------------------------------------------------------------- */
	//  strtod
	/* --------------------------------------------------------------------- */
	inline double strtod(const wchar_t* str, wchar_t** end) {
		return std::wcstod(str, end);
	}
	
	/* --------------------------------------------------------------------- */
	//  strftime
	/* --------------------------------------------------------------------- */
	inline std::size_t strftime(wchar_t* str, std::size_t n, const wchar_t* fmt, const std::tm* tmptr) {
		return std::wcsftime(str, n, fmt, tmptr);
	}
	
	/* --------------------------------------------------------------------- */
	//  strxfrm
	/* --------------------------------------------------------------------- */
	inline std::size_t strxfrm(wchar_t* dest, const wchar_t* src, std::size_t n) {
		return std::wcsxfrm(dest, src, n);
	}
#endif // CLX_USE_WCHAR
}

#endif // CLX_CSTRING_H
