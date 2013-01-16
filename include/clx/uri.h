/* ------------------------------------------------------------------------- */
/*
 *  uri.h
 *
 *  Copyright (c) 2004 - 2009, clown. All rights reserved.
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
 *  Last-modified: Tue 03 Nov 2009 21:53:03 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_URI_H
#define CLX_URI_H

#include "config.h"
#include <string>
#include <iterator>
#include "literal.h"
#include "lexical_cast.h"
#include "convert.h"
#include "uri_convert.h"

namespace clx {
	namespace uri {
		/* ----------------------------------------------------------------- */
		//  encode
		/* ----------------------------------------------------------------- */
		template <class InIter, class OutIter>
		inline OutIter encode(InIter first, InIter last, OutIter result) {
			clx::basic_uri_encoder<typename InIter::value_type> f;
			return clx::convert(first, last, result, f);
		}
		
		/* ----------------------------------------------------------------- */
		//  encode
		/* ----------------------------------------------------------------- */
		template <class Ch, class Tr>
		inline std::basic_string<Ch, Tr> encode(const std::basic_string<Ch, Tr>& src) {
			clx::basic_uri_encoder<Ch, Tr> f;
			return clx::convert(src, f);
		}
		
		/* ----------------------------------------------------------------- */
		//  encode
		/* ----------------------------------------------------------------- */
		template <class CharT>
		inline std::basic_string<CharT> encode(const CharT* src) {
			clx::basic_uri_encoder<CharT> f;
			return clx::convert(src, f);
		}
		
		/* ----------------------------------------------------------------- */
		//  decode
		/* ----------------------------------------------------------------- */
		template <class InIter, class OutIter>
		inline OutIter decode(InIter first, InIter last, OutIter result) {
			clx::basic_uri_decoder<typename InIter::value_type> f;
			return clx::convert(first, last, result, f);
		}
		
		/* ----------------------------------------------------------------- */
		//  decode
		/* ----------------------------------------------------------------- */
		template <class Ch, class Tr>
		inline std::basic_string<Ch, Tr> decode(const std::basic_string<Ch, Tr>& src) {
			clx::basic_uri_decoder<Ch, Tr> f;
			return clx::convert(src, f);
		}
		
		/* ----------------------------------------------------------------- */
		//  decode
		/* ----------------------------------------------------------------- */
		template <class CharT>
		inline std::basic_string<CharT> decode(const CharT* src) {
			clx::basic_uri_decoder<CharT> f;
			return clx::convert(src, f);
		}
	};
}

#endif
