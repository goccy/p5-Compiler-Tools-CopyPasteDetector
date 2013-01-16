/* ------------------------------------------------------------------------- */
/*
 *  utf16.h
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
 *  Last-modified: Wed 10 Mar 2010 20:32:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_UTF16_H
#define CLX_UTF16_H

#include "config.h"
#include <iterator>
#include <stdexcept>
#include "stdint.h"
#include "unicode.h"
#include "static_check.h"

namespace clx {
	namespace utf16 {
		/* ----------------------------------------------------------------- */
		//  get_unicode
		/* ----------------------------------------------------------------- */
		template <class InIter>
		inline char32_t get_unicode(InIter& first, InIter last) {
			STATIC_CHECK(sizeof(typename std::iterator_traits<InIter>::value_type) == sizeof(char16_t),
				allows_only_char16_t_in_input_value_type);
			if (first == last) return char32_t(-1);
			
			clx::uint16_t code = static_cast<clx::uint16_t>(*first++);
			if (code >= 0xd800 && code <= 0xdbff) {
				char32_t dest = static_cast<char32_t>(code & 0x3ff) << 10;
				if (first == last) return char32_t(-1);
				code = static_cast<clx::uint16_t>(*first++);
				if (!(code >= 0xdc00 && code <= 0xdfff)) return char32_t(-1);
				dest |= static_cast<char32_t>(code & 0x3ff);
				dest += 0x10000;
				return dest;
			}
			else return static_cast<char32_t>(code);
		}
		
		/* ----------------------------------------------------------------- */
		//  peek_unicode
		/* ----------------------------------------------------------------- */
		template <class InIter>
		inline char32_t peek_unicode(InIter first, InIter last) {
			InIter tmp = first;
			return clx::utf16::get_unicode(tmp, last);
		}
	}
}

#endif // CLX_UTF16_H
