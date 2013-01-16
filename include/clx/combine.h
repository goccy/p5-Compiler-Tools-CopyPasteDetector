/* ------------------------------------------------------------------------- */
/*
 *  combine.h
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
 *  Last-modified: Sat 31 Jul 2010 22:07:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_COMBINE_H
#define CLX_COMBINE_H

#include "config.h"
#include <climits>
#include "mpl/bitmask.h"
#if !defined(__BORLANDC__)
#include "range.h"
#endif

namespace clx {
#if !defined(__BORLANDC__)
	/* --------------------------------------------------------------------- */
	//  combine
	/* --------------------------------------------------------------------- */
	template <class Container>
	inline std::size_t combine(const Container& x) {
		std::size_t dest = 0;
		
		const std::size_t bits = sizeof(typename range_value<const Container>::type) * CHAR_BIT;
		typename range_iterator<const Container>::type pos = begin(x);
		typename range_iterator<const Container>::type last = end(x);
		for (; pos != last; ++pos) {
			std::size_t tmp = static_cast<std::size_t>(*pos) & mpl::lower_mask<bits>::value;
			dest <<= bits;
			dest |= tmp;
		}
		
		return dest;
	}
	
	/* --------------------------------------------------------------------- */
	//  combine
	/* --------------------------------------------------------------------- */
	template <class T1, class T2>
	inline std::size_t combine(const T1& x1, const T2& x2) {
		std::size_t tmp = 0, dest = 0;
		
		tmp = static_cast<std::size_t>(x1) & mpl::lower_mask<sizeof(T1) * CHAR_BIT>::value;
		dest |= tmp << (sizeof(T2) * CHAR_BIT);
		tmp = static_cast<std::size_t>(x2) & mpl::lower_mask<sizeof(T2) * CHAR_BIT>::value;
		dest |= tmp;
		
		return dest;
	}
	
	/* --------------------------------------------------------------------- */
	//  combine
	/* --------------------------------------------------------------------- */
	template <class T1, class T2, class T3>
	inline std::size_t combine(const T1& x1, const T2& x2, const T3& x3) {
		std::size_t tmp = 0, dest = 0;
		
		tmp = combine(x1, x2);
		dest |= tmp << (sizeof(T3) * CHAR_BIT);
		tmp = static_cast<std::size_t>(x3) & mpl::lower_mask<sizeof(T3) * CHAR_BIT>::value;
		dest |= tmp;
		
		return dest;
	}
	
	/* --------------------------------------------------------------------- */
	//  combine
	/* --------------------------------------------------------------------- */
	template <class T1, class T2, class T3, class T4>
	inline std::size_t combine(const T1& x1, const T2& x2, const T3& x3, const T4& x4) {
		std::size_t tmp = 0, dest = 0;
		
		tmp = combine(x1, x2, x3);
		dest |= tmp << (sizeof(T4) * CHAR_BIT);
		tmp = static_cast<std::size_t>(x4) & mpl::lower_mask<sizeof(T4) * CHAR_BIT>::value;
		dest |= tmp;
		
		return dest;
	}

#else // for Borland C++ Compiler
	
	/*
	 * Note: In the current version combine functions restrict only
	 * unsigned char type.
	 */
	 
	/* --------------------------------------------------------------------- */
	//  combine
	/* --------------------------------------------------------------------- */
	inline std::size_t combine(unsigned char x1, unsigned char x2) {
		std::size_t tmp = 0, dest = 0;
		
		tmp = static_cast<std::size_t>(x1) & mpl::lower_mask<sizeof(unsigned char) * CHAR_BIT>::value;
		dest |= tmp << (sizeof(unsigned char) * CHAR_BIT);
		tmp = static_cast<std::size_t>(x2) & mpl::lower_mask<sizeof(unsigned char) * CHAR_BIT>::value;
		dest |= tmp;
		
		return dest;
	}
	
	/* --------------------------------------------------------------------- */
	//  combine
	/* --------------------------------------------------------------------- */
	inline std::size_t combine(unsigned char x1, unsigned char x2, unsigned char x3) {
		std::size_t tmp = 0, dest = 0;
		
		tmp = combine(x1, x2);
		dest |= tmp << (sizeof(unsigned char) * CHAR_BIT);
		tmp = static_cast<std::size_t>(x3) & mpl::lower_mask<sizeof(unsigned char) * CHAR_BIT>::value;
		dest |= tmp;
		
		return dest;
	}
	
	/* --------------------------------------------------------------------- */
	//  combine
	/* --------------------------------------------------------------------- */
	inline std::size_t combine(unsigned char x1, unsigned char x2, unsigned char x3, unsigned char x4) {
		std::size_t tmp = 0, dest = 0;
		
		tmp = combine(x1, x2, x3);
		dest |= tmp << (sizeof(unsigned char) * CHAR_BIT);
		tmp = static_cast<std::size_t>(x4) & mpl::lower_mask<sizeof(unsigned char) * CHAR_BIT>::value;
		dest |= tmp;
		
		return dest;
	}
#endif
} // namespace clx

#endif // CLX_COMBINE_H
