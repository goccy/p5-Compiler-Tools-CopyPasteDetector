/* ------------------------------------------------------------------------- */
/*
 *  utf8/distance.h
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
 *  Last-modified: Tue 09 Mar 2010 13:01:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_UTF8_DISTANCE_H
#define CLX_UTF8_DISTANCE_H

#include <iterator>
#include <stdexcept>
#include <string>
#include "../static_check.h"
#include "utility.h"

namespace clx {
	namespace utf8 {
		/* ----------------------------------------------------------------- */
		//  distance
		/* ----------------------------------------------------------------- */
		template <class InIter>
		inline std::size_t distance(InIter first, InIter last) {
			STATIC_CHECK(sizeof(typename std::iterator_traits<InIter>::value_type) == 1, utf8_functions_allow_only_char_type);
			
			std::size_t dest = 0;
			while (first != last) {
				std::size_t n = clx::utf8::bytes(*first);
				if (n == size_t(-1)) throw std::runtime_error("invalid UTF-8 character");
				std::advance(first, n);
				++dest;
			}
			return dest;
		}
		
		/* ----------------------------------------------------------------- */
		//  distance
		/* ----------------------------------------------------------------- */
		inline std::size_t distance(const std::basic_string<char>& src) {
			return clx::utf8::distance(src.begin(), src.end());
		}
		
		/* ----------------------------------------------------------------- */
		//  distance
		/* ----------------------------------------------------------------- */
		inline std::size_t distance(const char* src) {
			std::basic_string<char> tmp(src);
			return clx::utf8::distance(tmp);
		}
	}
}

#endif // CLX_UTF8_DISTANCE_H
