/* ------------------------------------------------------------------------- */
/*
 *  config.h
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
 *  Last-modified: Tue 16 Mar 2010 17:06:03 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_CONFIG_H
#define CLX_CONFIG_H

#include <climits>

#ifndef CLX_WIN32

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#define CLX_WIN32
#elif defined(_MSC_VER) && (_MSC_VER >= 1200)
#define CLX_WIN32
#endif

/* ------------------------------------------------------------------------- */
//  Configurations about Visual C++
/* ------------------------------------------------------------------------- */
#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE 1
#endif

// include only winsock2.h
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif

#pragma warning(disable: 4355)
#pragma warning(disable: 4996)

#ifndef NOMINMAX
#define NOMINMAX
#endif
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#endif // CLX_WIN32

/* ------------------------------------------------------------------------- */
//  Configurations about Borland
/* ------------------------------------------------------------------------- */
#if defined(__BORLANDC__)
#pragma warn -8004 // warning about initialize
#pragma warn -8022 // warning about override
#pragma warn -8026 // warning about inline
#pragma warn -8027 // warning about inline
#endif // defined(__BORLANDC__)

// check long long (or __int64)
#if !defined(__BORLANDC__) // NOTE: BCC 5.5 cannot recognize the following huge number.
#if (defined(ULLONG_MAX) && (ULLONG_MAX == 18446744073709551615ULL)) || \
    (defined(ULONG_LONG_MAX) && ULONG_LONG_MAX == 18446744073709551615ULL) || \
    (defined(ULONGLONG_MAX) && ULONGLONG_MAX == 18446744073709551615ULL) || \
    (defined(_LLONG_MAX) && _LLONG_MAX == 18446744073709551615ULL) || \
    (defined(_MSC_VER) && (_MSC_VER >= 1200)) || (defined(_UI64_MAX))
#define CLX_HAS_LONG_LONG 1
#endif
#endif

#ifndef CLX_ENABLE_AUTOLINK
#define CLX_ENABLE_AUTOLINK
#endif
#ifdef CLX_DISABLE_AUTOLINK
#undef CLX_ENABLE_AUTOLINK
#endif

#endif // CLX_CONFIG_H
