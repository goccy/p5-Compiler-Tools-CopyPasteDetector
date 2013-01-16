/* ------------------------------------------------------------------------- */
/*
 *  utf8/utility.h
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
#ifndef CLX_UTF8_UTILITY_H
#define CLX_UTF8_UTILITY_H

#include <climits>
#include <iterator>
#include <string>
#include "../stdint.h"

namespace clx {
	typedef ::clx::uint32_t utf8_t;
	
	namespace utf8 {
		/* ----------------------------------------------------------------- */
		//  bytes
		/* ----------------------------------------------------------------- */
		inline std::size_t bytes(unsigned char c) {
			std::size_t n = 0;
			
			if (c <= 0x7f) n = 1;
			else if (c >= 0xc2 && c <= 0xdf) n = 2;
			else if (c >= 0xe0 && c <= 0xef) n = 3;
			else if (c >= 0xf0 && c <= 0xf7) n = 4;
			else n = std::size_t(-1);
			
			return n;
		}
		
		/* ----------------------------------------------------------------- */
		//  convert
		/* ----------------------------------------------------------------- */
		template <class OutIter>
		inline OutIter convert(clx::utf8_t code, OutIter result) {
			static const std::size_t maxlen = 4;
			
			unsigned char buffer[maxlen];
			buffer[3] = (code >> CHAR_BIT * 0) & 0xff;
			buffer[2] = (code >> CHAR_BIT * 1) & 0xff;
			buffer[1] = (code >> CHAR_BIT * 2) & 0xff;
			buffer[0] = (code >> CHAR_BIT * 3) & 0xff;
			
			std::size_t i = 0;
			while (i < maxlen - 1 && buffer[i] == 0) ++i;
			while (i < maxlen) *result++ = buffer[i++];
			
			return result;
		}
		
		/* ----------------------------------------------------------------- */
		//  convert
		/* ----------------------------------------------------------------- */
		inline std::basic_string<char> convert(clx::utf8_t code) {
			std::basic_string<char> dest;
			std::insert_iterator<std::basic_string<char> > out(dest, dest.end());
			clx::utf8::convert(code, out);
			return dest;
		}
	}
}

#endif // CLX_UTF8_UTILITY_H
