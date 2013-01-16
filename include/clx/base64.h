/* ------------------------------------------------------------------------- */
/*
 *  base64.h
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
 *  Last-modified: Fri 30 Jul 2010 20:03:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_BASE64_H
#define CLX_BASE64_H

#include "config.h"
#include <iterator>
#include <cstring>
#include <string>
#include <stdexcept>
#include "convert.h"
#include "base64_convert.h"

namespace clx {
	namespace base64 {
		/* ----------------------------------------------------------------- */
		//  encode
		/* ----------------------------------------------------------------- */
		template <class InIter, class OutIter>
		inline OutIter encode(InIter first, InIter last, OutIter result) {
#if defined(__BORLANDC__)
			typedef char value_type;
#else
			typedef typename InIter::value_type value_type;
#endif
			clx::basic_base64_encoder<value_type> f;
			return clx::convert(first, last, result, f);
		}
		
		/* ----------------------------------------------------------------- */
		//  encode
		/* ----------------------------------------------------------------- */
		inline std::basic_string<char> encode(const std::basic_string<char>& src) {
			if (src.empty()) return std::basic_string<char>();
			std::basic_string<char> dest;
			std::insert_iterator<std::basic_string<char> > out(dest, dest.begin());
			clx::base64::encode(src.begin(), src.end(), out);
			return dest;
		}
		
		/* ----------------------------------------------------------------- */
		//  encode
		/* ----------------------------------------------------------------- */
		inline std::basic_string<char> encode(const char* src) {
			std::basic_string<char> tmp(src);
			return clx::base64::encode(tmp);
		}
		
		/* ----------------------------------------------------------------- */
		/*
		 *  encode
		 *
		 *  The function is deprecated. Use encode(InIter, InIter, OutIter)
		 */
		/* ----------------------------------------------------------------- */
		inline std::basic_string<char> encode(const char* src, std::size_t n) {
			if (n == 0) return std::basic_string<char>();
			std::basic_string<char> tmp(src, n);
			return clx::base64::encode(tmp);
		}
		
		/* ----------------------------------------------------------------- */
		//  decode
		/* ----------------------------------------------------------------- */
		template <class InIter, class OutIter>
		inline OutIter decode(InIter first, InIter last, OutIter result) {
#if defined(__BORLANDC__)
			typedef char value_type;
#else
			typedef typename InIter::value_type value_type;
#endif
			clx::basic_base64_decoder<value_type> f;
			return clx::convert(first, last, result, f);
		}
		
		/* ----------------------------------------------------------------- */
		//  decode
		/* ----------------------------------------------------------------- */
		inline std::basic_string<char> decode(const std::basic_string<char>& src) {
			if (src.empty()) return std::basic_string<char>();
			std::basic_string<char> dest;
			std::insert_iterator<std::basic_string<char> > out(dest, dest.begin());
			clx::base64::decode(src.begin(), src.end(), out);
			return dest;
		}
		
		/* ----------------------------------------------------------------- */
		//  decode
		/* ----------------------------------------------------------------- */
		inline std::basic_string<char> decode(const char* src) {
			std::basic_string<char> tmp(src);
			return clx::base64::decode(tmp);
		}
		
		/* ----------------------------------------------------------------- */
		/*
		 *  decode
		 *
		 *  The function is deprecated. Use decode(InIter, InIter, OutIter)
		 *  or decode(const char*).
		 */
		/* ----------------------------------------------------------------- */
		inline std::basic_string<char> decode(const char* src, size_t n) {
			if (n == 0) return std::basic_string<char>();
			std::basic_string<char> tmp(src, n);
			return clx::base64::decode(tmp);
		}
	}
}

#endif // CLX_BASE64_H
