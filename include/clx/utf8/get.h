/* ------------------------------------------------------------------------- */
/*
 *  utf8/get.h
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
#ifndef CLX_UTF8_GET_H
#define CLX_UTF8_GET_H

#include <iterator>
#include <stdexcept>
#include "../static_check.h"
#include "utility.h"
#include "encode.h"

namespace clx {
	namespace utf8 {
		/* ----------------------------------------------------------------- */
		//  get
		/* ----------------------------------------------------------------- */
		template <class InIter>
		inline clx::utf8_t get(InIter& first, InIter last) {
			STATIC_CHECK(sizeof(typename std::iterator_traits<InIter>::value_type) == sizeof(char),
				utf8_functions_allow_only_char_type);
			if (first == last) return clx::utf8_t(-1);
			
			std::size_t n = clx::utf8::bytes(*first);
			if (n == std::size_t(-1)) throw std::runtime_error("invalid UTF-8 character");
			
			clx::utf8_t dest = 0;
			for (std::size_t i = 0; i < n && first != last; ++i, ++first) {
				clx::utf8_t c = static_cast<clx::utf8_t>(*first) & 0xff;
				if (i > 0 && (c & 0xc0) != 0x80) throw std::runtime_error("invalid UTF-8 character");
				dest <<= 8;
				dest |= c;
			}
			
			return dest;
		}
		
		/* ----------------------------------------------------------------- */
		//  get
		/* ----------------------------------------------------------------- */
		template <class InIter, class OutIter>
		inline OutIter get(InIter& first, InIter last, OutIter result) {
			clx::utf8_t c = clx::utf8::get(first, last);
			return (c == clx::utf8_t(-1)) ? result : clx::utf8::convert(c, result);
		}
		
		/* ----------------------------------------------------------------- */
		//  get_unicode
		/* ----------------------------------------------------------------- */
		template <class InIter>
		inline char32_t get_unicode(InIter& first, InIter last) {
			clx::utf8_t c = clx::utf8::get(first, last);
			return (c == clx::utf8_t(-1)) ? char32_t(-1) : clx::utf8::decode(c);
		}
		
		/* ----------------------------------------------------------------- */
		//  peek
		/* ----------------------------------------------------------------- */
		template <class InIter>
		inline clx::utf8_t peek(InIter first, InIter last) {
			InIter tmp = first;
			return clx::utf8::get(tmp, last);
		}
		
		/* ----------------------------------------------------------------- */
		//  peek_unicode
		/* ----------------------------------------------------------------- */
		template <class InIter>
		inline char32_t peek_unicode(InIter first, InIter last) {
			InIter tmp = first;
			return clx::utf8::get_unicode(tmp, last);
		}
	}
}

#endif // CLX_UTF8_GET_H
