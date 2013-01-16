/* ------------------------------------------------------------------------- */
/*
 *  md5.h
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
 *  Copyright (c) 1991-2, RSA Data Security, Inc. Created 1991.
 *  All rights reserved.
 *
 *  The source code is derived from the RSA Data Security, Inc. MD5
 *  Message-Digest Algorithm. (See the following URL:
 *  http://www.faqs.org/rfcs/rfc1321.html)
 *
 *  Last-modified: Thu 28 Jan 2010 05:39:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_MD5_H
#define CLX_MD5_H

#include "config.h"
#include <cstring>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include "memory.h"
#include "rotate.h"

// F, G, H and I are basic MD5 functions.
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

#define ROUND1(a, b, c, d, x, s, ac) { \
 (a) += F((b), (c), (d)) + (x) + (uint32_t)(ac); \
 (a) = clx::rotate_copy((a), (s)); \
 (a) += (b); \
  }
#define ROUND2(a, b, c, d, x, s, ac) { \
 (a) += G((b), (c), (d)) + (x) + (uint32_t)(ac); \
 (a) = clx::rotate_copy((a), (s)); \
 (a) += (b); \
  }
#define ROUND3(a, b, c, d, x, s, ac) { \
 (a) += H((b), (c), (d)) + (x) + (uint32_t)(ac); \
 (a) = clx::rotate_copy((a), (s)); \
 (a) += (b); \
  }
#define ROUND4(a, b, c, d, x, s, ac) { \
 (a) += I((b), (c), (d)) + (x) + (uint32_t)(ac); \
 (a) = clx::rotate_copy((a), (s)); \
 (a) += (b); \
  }

namespace clx {
	/* --------------------------------------------------------------------- */
	//  md5
	/* --------------------------------------------------------------------- */
	class md5 {
	public:
		typedef unsigned int uint32_t;
		typedef unsigned char uchar_t;
		typedef char char_type;
		typedef std::string string_type;
		
		md5() { this->reset(); }
		
		explicit md5(const char_type* src, size_t n) {
			this->encode(src, n);
		}
		
		explicit md5(const string_type& src) {
			this->encode(src);
		}
		
		virtual ~md5() {}
		
		md5& encode(const char_type* src, size_t n) {
			this->reset();
			this->update(src, n);
			this->finish();
			return *this;
		}
		
		md5& encode(const string_type& src) {
			return this->encode(src.c_str(), src.size());
		}
		
		void reset() {
			std::memset(code_, 0, sizeof(code_));
			std::memset(buffer_, 0, sizeof(buffer_));
			
			count_[0] = 0;
			count_[1] = 0;
			
			w_[0] = 0x67452301;
			w_[1] = 0xefcdab89;
			w_[2] = 0x98badcfe;
			w_[3] = 0x10325476;
		}
		
		void update(const char_type* src, std::size_t n) {
			if (n == 0) return;
			uchar_t* block = (uchar_t*)src;
			
			uint32_t left = count_[0] & 0x3f;
			uint32_t fill = 64 - left;
			
			count_[0] += static_cast<uint32_t>(n);
			count_[0] &= 0xffffffff;
			if (count_[0] < n) ++count_[1];
			
			if (left && n >= fill) {
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
			
			if (n > 0) clx::memcpy(buffer_ + left, block, static_cast<int>(n));
		}
		
		void finish() {
			uint32_t high = (count_[0] >> 29) | (count_[1] << 3);
			uint32_t low = (count_[0] << 3);
			uchar_t nbit[8];
			clx::memcpy(nbit, (uchar_t*)&low, 4);
			clx::memcpy(&nbit[4], (uchar_t*)&high, 4);
			
			uint32_t last = count_[0] & 0x3f;
			uint32_t padn = (last < 56) ? (56 - last) : (120 - last);
			
			uchar_t padding[64];
			std::memset(padding, 0, sizeof(padding));
			padding[0] = 0x80;
			this->update((char*)padding, padn);
			this->update((char*)nbit, 8);
			
			clx::memcpy(code_, (uchar_t*)w_, sizeof(w_));
			
			// zeroize sensitive information
			std::memset(buffer_, 0, sizeof(buffer_));
		}
		
		std::size_t size() const { return sizeof(code_) - 1; }
		const uchar_t* code() const { return code_; }
		
		string_type to_string() const {
			std::basic_stringstream<char_type> ss;
			for (int i = 0; i < 16; ++i) {
				ss << std::setfill('0') << std::setw(2) << std::hex << (uint32_t)code_[i];
			}
			return ss.str();
		}
		
	private:
		uchar_t code_[17]; // message digest (32bit * 4 = 128bit)
		
		// working fields
		uint32_t w_[4];
		uint32_t count_[2];
		uchar_t buffer_[64];
		
		static void calculate(const uchar_t* block, uint32_t* code) {
			uint32_t x[16];
			clx::memcpy((uchar_t*)x, block, sizeof(x));
			
			uint32_t a = code[0];
			uint32_t b = code[1];
			uint32_t c = code[2];
			uint32_t d = code[3];
			
			ROUND1(a, b, c, d, x[ 0],  7, 0xd76aa478);
			ROUND1(d, a, b, c, x[ 1], 12, 0xe8c7b756);
			ROUND1(c, d, a, b, x[ 2], 17, 0x242070db);
			ROUND1(b, c, d, a, x[ 3], 22, 0xc1bdceee);
			ROUND1(a, b, c, d, x[ 4],  7, 0xf57c0faf);
			ROUND1(d, a, b, c, x[ 5], 12, 0x4787c62a);
			ROUND1(c, d, a, b, x[ 6], 17, 0xa8304613);
			ROUND1(b, c, d, a, x[ 7], 22, 0xfd469501);
			ROUND1(a, b, c, d, x[ 8],  7, 0x698098d8);
			ROUND1(d, a, b, c, x[ 9], 12, 0x8b44f7af);
			ROUND1(c, d, a, b, x[10], 17, 0xffff5bb1);
			ROUND1(b, c, d, a, x[11], 22, 0x895cd7be);
			ROUND1(a, b, c, d, x[12],  7, 0x6b901122);
			ROUND1(d, a, b, c, x[13], 12, 0xfd987193);
			ROUND1(c, d, a, b, x[14], 17, 0xa679438e);
			ROUND1(b, c, d, a, x[15], 22, 0x49b40821);
			
			ROUND2(a, b, c, d, x[ 1],  5, 0xf61e2562);
			ROUND2(d, a, b, c, x[ 6],  9, 0xc040b340);
			ROUND2(c, d, a, b, x[11], 14, 0x265e5a51);
			ROUND2(b, c, d, a, x[ 0], 20, 0xe9b6c7aa);
			ROUND2(a, b, c, d, x[ 5],  5, 0xd62f105d);
			ROUND2(d, a, b, c, x[10],  9, 0x02441453);
			ROUND2(c, d, a, b, x[15], 14, 0xd8a1e681);
			ROUND2(b, c, d, a, x[ 4], 20, 0xe7d3fbc8);
			ROUND2(a, b, c, d, x[ 9],  5, 0x21e1cde6);
			ROUND2(d, a, b, c, x[14],  9, 0xc33707d6);
			ROUND2(c, d, a, b, x[ 3], 14, 0xf4d50d87);
			ROUND2(b, c, d, a, x[ 8], 20, 0x455a14ed);
			ROUND2(a, b, c, d, x[13],  5, 0xa9e3e905);
			ROUND2(d, a, b, c, x[ 2],  9, 0xfcefa3f8);
			ROUND2(c, d, a, b, x[ 7], 14, 0x676f02d9);
			ROUND2(b, c, d, a, x[12], 20, 0x8d2a4c8a);
			
			ROUND3(a, b, c, d, x[ 5],  4, 0xfffa3942);
			ROUND3(d, a, b, c, x[ 8], 11, 0x8771f681);
			ROUND3(c, d, a, b, x[11], 16, 0x6d9d6122);
			ROUND3(b, c, d, a, x[14], 23, 0xfde5380c);
			ROUND3(a, b, c, d, x[ 1],  4, 0xa4beea44);
			ROUND3(d, a, b, c, x[ 4], 11, 0x4bdecfa9);
			ROUND3(c, d, a, b, x[ 7], 16, 0xf6bb4b60);
			ROUND3(b, c, d, a, x[10], 23, 0xbebfbc70);
			ROUND3(a, b, c, d, x[13],  4, 0x289b7ec6);
			ROUND3(d, a, b, c, x[ 0], 11, 0xeaa127fa);
			ROUND3(c, d, a, b, x[ 3], 16, 0xd4ef3085);
			ROUND3(b, c, d, a, x[ 6], 23,  0x4881d05);
			ROUND3(a, b, c, d, x[ 9],  4, 0xd9d4d039);
			ROUND3(d, a, b, c, x[12], 11, 0xe6db99e5);
			ROUND3(c, d, a, b, x[15], 16, 0x1fa27cf8);
			ROUND3(b, c, d, a, x[ 2], 23, 0xc4ac5665);
			
			ROUND4(a, b, c, d, x[ 0],  6, 0xf4292244);
			ROUND4(d, a, b, c, x[ 7], 10, 0x432aff97);
			ROUND4(c, d, a, b, x[14], 15, 0xab9423a7);
			ROUND4(b, c, d, a, x[ 5], 21, 0xfc93a039);
			ROUND4(a, b, c, d, x[12],  6, 0x655b59c3);
			ROUND4(d, a, b, c, x[ 3], 10, 0x8f0ccc92);
			ROUND4(c, d, a, b, x[10], 15, 0xffeff47d);
			ROUND4(b, c, d, a, x[ 1], 21, 0x85845dd1);
			ROUND4(a, b, c, d, x[ 8],  6, 0x6fa87e4f);
			ROUND4(d, a, b, c, x[15], 10, 0xfe2ce6e0);
			ROUND4(c, d, a, b, x[ 6], 15, 0xa3014314);
			ROUND4(b, c, d, a, x[13], 21, 0x4e0811a1);
			ROUND4(a, b, c, d, x[ 4],  6, 0xf7537e82);
			ROUND4(d, a, b, c, x[11], 10, 0xbd3af235);
			ROUND4(c, d, a, b, x[ 2], 15, 0x2ad7d2bb);
			ROUND4(b, c, d, a, x[ 9], 21, 0xeb86d391);
			
			code[0] += a;
			code[1] += b;
			code[2] += c;
			code[3] += d;
			
			// zeroize sensitive information.
			std::memset(x, 0, sizeof(x));
		}
	};
}

#endif // CLX_MD5_H
