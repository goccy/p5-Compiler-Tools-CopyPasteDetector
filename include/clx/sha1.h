/* ------------------------------------------------------------------------- */
/*
 *  sha1.h
 *
 *  Copyright (c) 2004 - 2008, clown. All rights reserved.
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
 *  Last-modified: Sat 14 Mar 2009 21:19:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_SHA1_H
#define CLX_SHA1_H

#include "config.h"
#include <cstring>
#include <climits>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include "memory.h"
#include "reverse.h"
#include "rotate.h"

namespace clx {
	/* --------------------------------------------------------------------- */
	//  sha1
	/* --------------------------------------------------------------------- */
	class sha1 {
	public:
		typedef unsigned int uint32_t;
		typedef unsigned char uchar_t;
		typedef char char_type;
		typedef std::string string_type;
		
		sha1() { this->reset(); }
		
		explicit sha1(const char_type* src, size_t n) {
			this->encode(src, n);
		}
		
		explicit sha1(const string_type& src) {
			this->encode(src);
		}
		
		virtual ~sha1() {}
		
		sha1& encode(const char_type* src, size_t n) {
			this->reset();
			this->update(src, n);
			this->finish();
			return *this;
		}
		
		sha1& encode(const string_type& src) {
			return this->encode(src.c_str(), src.size());
		}
		
		void reset() {
			std::memset(buffer_, 0, sizeof(buffer_));
			std::memset(code_, 0, sizeof(code_));
			
			count_[0] = 0;
			count_[1] = 0;
			
			w_[0] = 0x67452301;
			w_[1] = 0xefcdab89;
			w_[2] = 0x98badcfe;
			w_[3] = 0x10325476;
			w_[4] = 0xc3d2e1f0;
		}
		
		void update(const char_type* src, size_t n) {
			if (n == 0) return;
			uchar_t* block = (uchar_t*)src;
			
			uint32_t left = count_[0] & 0x3f;
			uint32_t fill = 64 - left;
			
			count_[0] += static_cast<uint32_t>(n);
			count_[0] &= 0xffffffff;
			if (count_[0] < n) ++count_[1];
			
			if (left > 0 && n >= fill) {
				clx::memcpy(buffer_ + left, block, fill);
				this->calculate(buffer_, w_);
				n -= fill;
				block += fill;
				left = 0;
			}
			
			while (n >= 64) {
				this->calculate(block, w_);
				n -= 64;
				block += 64;
			}
			
			if (n > 0) clx::memcpy(buffer_ + left, block, n);
		}
		
		void finish() {
			uint32_t high = reverse_copy((count_[0] >> 29) | (count_[1] << 3));
			uint32_t low = reverse_copy(count_[0] << 3);
			uchar_t nbit[8];
			clx::memcpy(nbit, &high, 4);
			clx::memcpy(&nbit[4], &low, 4);
			
			uint32_t last = count_[0] & 0x3f;
			uint32_t padn = (last < 56) ? (56 - last) : (120 - last);
			
			uchar_t padding[64];
			std::memset(padding, 0, sizeof(padding));
			padding[0] = 0x80;
			this->update((char*)padding, padn);
			this->update((char*)nbit, 8);
			
			for (int i = 0, j = 0; i < 5; ++i) {
				code_[j++] = (uchar_t)((w_[i] >> 24) & 0x000000ff);
				code_[j++] = (uchar_t)((w_[i] >> 16) & 0x000000ff);
				code_[j++] = (uchar_t)((w_[i] >> 8) & 0x000000ff);
				code_[j++] = (uchar_t)(w_[i] & 0x000000ff);
			}
			
			// zeroize sensitive information
			std::memset(buffer_, 0, sizeof(buffer_));
		}
		
		int size() const { return sizeof(code_) - 1; }
		const uchar_t* code() const { return code_; }
		
		string_type to_string() const {
			std::basic_stringstream<char_type> ss;
			for (int i = 0; i < 5; ++i) {
				ss << std::setfill('0') << std::setw(8) << std::hex << w_[i];
			}
			return ss.str();
		}
		
	private:
		uchar_t code_[21]; // message digest (32bit * 5 = 160bit)
		
		// working fields
		uint32_t w_[5];
		uint32_t count_[2];
		uchar_t buffer_[64];
		
		static uint32_t f(int t, uint32_t b, uint32_t c, uint32_t d) {
			if (t < 20) return (b & c) | (~b & d);
			else if (t < 40) return b ^ c ^ d;
			else if (t < 60) return (b & c) | (b & d) | (c & d);
			else return b ^ c ^ d;
		}
		
		static uint32_t k(int t) {
			if (t < 20) return 0x5a827999;
			else if (t < 40) return 0x6ed9eba1;
			else if (t < 60) return 0x8f1bbcdc;
			else return 0xca62c1d6;
		}
		
		static void calculate(const uchar_t* block, uint32_t* code) {
			uint32_t x[16];
			clx::memcpy(x, block, sizeof(x));
			
			uint32_t w[80];
			for (int i = 0; i < 16; ++i) w[i] = reverse_copy(x[i]);
			for (int i = 16; i < 80; ++i) w[i] = rotate_copy(w[i-3] ^ w[i-8] ^ w[i-14] ^ w[i-16], 1);
			
			uint32_t a = code[0];
			uint32_t b = code[1];
			uint32_t c = code[2];
			uint32_t d = code[3];
			uint32_t e = code[4];
			
			for (int t = 0; t < 80; ++t) {
				uint32_t tmp = rotate_copy(a,5) + f(t,b,c,d) + e + w[t] + k(t);
				e = d;
				d = c;
				c = rotate_copy(b, 30);
				b = a;
				a = tmp;
			}
			
			code[0] += a;
			code[1] += b;
			code[2] += c;
			code[3] += d;
			code[4] += e;
			
			// zeroize sensitive information.
			std::memset(x, 0, sizeof(x));
		}
	};
}

#endif // CLX_SHA1_H

