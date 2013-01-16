/* ------------------------------------------------------------------------- */
/*
 *  stdint_alternative.h
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
 *  Last-modified: Fri 19 Mar 2010 20:22:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_STDINT_ALTERNATIVE_H
#define CLX_STDINT_ALTERNATIVE_H

#include <climits>

namespace clx {
	/* --------------------------------------------------------------------- */
	//  8-bit types
	/* --------------------------------------------------------------------- */
#if UCHAR_MAX == 0xff
	typedef signed char   int8_t;
	typedef signed char   int_least8_t;
	typedef signed char   int_fast8_t;
	typedef unsigned char uint8_t;
	typedef unsigned char uint_least8_t;
	typedef unsigned char uint_fast8_t;
#else
#error defaults not correct; you must hand modify clx/stdint_alternative.h
#endif
	
	/* --------------------------------------------------------------------- */
	//  16-bit types
	/* --------------------------------------------------------------------- */
#if USHRT_MAX == 0xffff
	typedef signed short   int16_t;
	typedef signed short   int_least16_t;
	typedef signed short   int_fast16_t;
	typedef unsigned short uint16_t;
	typedef unsigned short uint_least16_t;
	typedef unsigned short uint_fast16_t;
#else
#error defaults not correct; you must hand modify clx/stdint_alternative.h
#endif
	
	/* --------------------------------------------------------------------- */
	//  32-bit types
	/* --------------------------------------------------------------------- */
#if ULONG_MAX == 0xffffffff
	typedef signed long   int32_t;
	typedef signed long   int_least32_t;
	typedef signed long   int_fast32_t;
	typedef unsigned long uint32_t;
	typedef unsigned long uint_least32_t;
	typedef unsigned long uint_fast32_t;
#elif UINT_MAX == 0xffffffff
	typedef signed int    int32_t;
	typedef signed int    int_least32_t;
	typedef signed int    int_fast32_t;
	typedef unsigned int  uint32_t;
	typedef unsigned int  uint_least32_t;
	typedef unsigned int  uint_fast32_t;
#else
#error defaults not correct; you must hand modify clx/stdint_alternative.h
#endif
	
	/* --------------------------------------------------------------------- */
	//  64-bit types and intmax_t/uintmax_t
	/* --------------------------------------------------------------------- */
#if defined(CLX_HAS_LONG_LONG)
#if (defined(_MSC_VER) && (_MSC_VER >= 1200)) || (defined(_UI64_MAX))
	typedef signed __int64                   int64_t;
	typedef signed __int64                   int_least64_t;
	typedef signed __int64                   int_fast64_t;
	typedef unsigned __int64                 uint64_t;
	typedef unsigned __int64                 uint_least64_t;
	typedef unsigned __int64                 uint_fast64_t;
#elif defined(__GNUC__)
	__extension__ typedef signed long long   int64_t;
	__extension__ typedef signed long long   int_least64_t;
	__extension__ typedef signed long long   int_fast64_t;
	__extension__ typedef unsigned long long uint64_t;
	__extension__ typedef unsigned long long uint_least64_t;
	__extension__ typedef unsigned long long uint_fast64_t;
#else
    typedef signed long long                 int64_t;
    typedef signed long long                 int_least64_t;
    typedef signed long long                 int_fast64_t;
    typedef unsigned long long               uint64_t;
    typedef unsigned long long               uint_least64_t;
    typedef unsigned long long               uint_fast64_t;
#endif
	typedef int64_t                          intmax_t;
	typedef uint64_t                         uintmax_t;
#elif ULONG_MAX != 0xffffffff
#if ULONG_MAX == 18446744073709551615U // 2**64 - 1
	typedef signed long                      int64_t;
	typedef signed long                      int_least64_t;
	typedef signed long                      int_fast64_t;
	typedef unsigned long                    uint64_t;
	typedef unsigned long                    uint_least64_t;
	typedef unsigned long                    uint_fast64_t;
	typedef int64_t                          intmax_t;
	typedef uint64_t                         uintmax_t;
#else
#error defaults not correct; you must hand modify clx/stdint_alternative.h
#endif // ULONG_MAX == 18446744073709551615U
#else // assume no 64-bit integers
#define CLX_NO_INT64_T 1
	typedef int32_t                          intmax_t;
	typedef uint32_t                         uintmax_t;
#endif
}

#endif // CLX_STDINT_ALTERNATIVE_H
