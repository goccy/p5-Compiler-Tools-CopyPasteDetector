/* ------------------------------------------------------------------------- */
/*
 *  utf8/encode.h
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
 *  Last-modified: Wed 02 Jun 2010 05:06:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_UTF8_ENCODE_H
#define CLX_UTF8_ENCODE_H

#include <climits>
#include <stdexcept>
#include "../unicode.h"

namespace clx {
	namespace utf8 {
		/* ----------------------------------------------------------------- */
		/*
		 *  encode
		 *
		 *  The function encodes a Unicode (UTF-16) character to UTF-8.
		 */
		/* ----------------------------------------------------------------- */
		inline clx::utf8_t encode(char32_t code) {
			clx::utf8_t dest = 0;
			
			clx::uint32_t uc = static_cast<clx::uint32_t>(code);
			if (uc < 0x000080) { // [U+0000, U+007F]
				dest = static_cast<clx::utf8_t>(uc) & 0xff;
			}
			else if (uc < 0x000800) { // [U+0080, U+07FF]
				unsigned char c1 = static_cast<unsigned char>(0xc0 | (uc >> 6));
				unsigned char c0 = static_cast<unsigned char>(0x80 | (uc & 0x003f));
				dest = (c1 << CHAR_BIT) | c0;
			}
			else if (uc < 0x010000) { // [U+0800, U+FFFF]
				if (uc >= 0x00d800 && uc <=  0x00dfff) {
					throw std::runtime_error("invalid Unicode character");
				}
				unsigned char c2 = static_cast<unsigned char>(0xe0 | (uc >> 12));
				unsigned char c1 = static_cast<unsigned char>(0x80 | ((uc >> 6) & 0x3f));
				unsigned char c0 = static_cast<unsigned char>(0x80 | ((uc >> 0) & 0x3f));
				dest = (c2 << CHAR_BIT * 2) | (c1 << CHAR_BIT) | c0;
			}
			else if (uc < 0x110000) { // [U+010000, U+10FFFF]
				unsigned char c3 = static_cast<unsigned char>(0xf0 | (uc >> 18));
				unsigned char c2 = static_cast<unsigned char>(0x80 | ((uc >> 12) & 0x3f));
				unsigned char c1 = static_cast<unsigned char>(0x80 | ((uc >>  6) & 0x3f));
				unsigned char c0 = static_cast<unsigned char>(0x80 | ((uc >>  0) & 0x3f));
				dest = (c3 << CHAR_BIT * 3) | (c2 << CHAR_BIT * 2) | (c1 << CHAR_BIT) | c0;
			}
			else throw std::runtime_error("invalid Unicode character");
			
			return dest;
		}
		
		/* ----------------------------------------------------------------- */
		//  encode
		/* ----------------------------------------------------------------- */
		template <class OutIter>
		inline OutIter encode(char32_t code, OutIter result) {
			clx::utf8_t c = clx::utf8::encode(code);
			return clx::utf8::convert(c, result);
		}
		
		/* ----------------------------------------------------------------- */
		//  encode
		/* ----------------------------------------------------------------- */
		inline clx::utf8_t encode(char16_t code) {
			if (static_cast<unsigned int>(code) >= 0xd800 && static_cast<unsigned int>(code) <= 0xdfff) {
				throw std::runtime_error("surrogates pair is not currently supported");
			}
			return clx::utf8::encode(static_cast<char32_t>(code) & 0xffff);
		}
		
		/* ----------------------------------------------------------------- */
		//  encode
		/* ----------------------------------------------------------------- */
		template <class OutIter>
		inline OutIter encode(char16_t code, OutIter result) {
			clx::utf8_t c = clx::utf8::encode(code);
			return clx::utf8::convert(c, result);
		}
		
		/* ----------------------------------------------------------------- */
		/*
		 *  decode
		 *
		 *  The function decodes a UTF-8 character to Unicode (UTF-16).
		 */
		/* ----------------------------------------------------------------- */
		inline char32_t decode(clx::utf8_t code) {
			static const std::size_t mask[] = { 0x07, 0x0f, 0x1f, 0x7f };
			static const std::size_t maxlen = 4;
			
			unsigned char buffer[maxlen];
			buffer[3] = (code >> CHAR_BIT * 0) & 0xff;
			buffer[2] = (code >> CHAR_BIT * 1) & 0xff;
			buffer[1] = (code >> CHAR_BIT * 2) & 0xff;
			buffer[0] = (code >> CHAR_BIT * 3) & 0xff;
			
			std::size_t i = 0;
			while (i < maxlen && buffer[i] == 0) ++i;
			
			// valid utf-8 lead byte (0xf5 - 0xf7 is not currently used).
			if (!((buffer[i] >= 0x00 && buffer[i] <= 0x7f) ||
				  (buffer[i] >= 0xc2 && buffer[i] <= 0xf4))) {
				throw std::runtime_error("invalid UTF-8 character (found at lead byte)");
			}
			
			char32_t dest = (buffer[i] & mask[i]);
			++i;
			
			bool valid = (dest != 0) ? true : false;
			while (i < maxlen) {
				if ((!valid && buffer[i] == 0x80) || (buffer[i] & 0xc0) != 0x80) {
					throw std::runtime_error("invalid UTF-8 character (found at trail byte)");
				}
				dest <<= 6;
				dest |= (buffer[i++] & 0x3f);
				valid = true;
			}
			
			// surrogates pair ?
			if (dest >= 0xd800 && dest <= 0xdfff) {
				throw std::runtime_error("invalid Unicode character (surrogates pair ?)");
			}
			
			return dest;
		}
	}
}

#endif // CLX_UTF8_ENCODE_H
