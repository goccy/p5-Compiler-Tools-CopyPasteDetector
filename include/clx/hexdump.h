/* ------------------------------------------------------------------------- */
/*
 *  hexdump.h
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
 *  Last-modified: Mon 01 Mar 2010 19:43:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_HEXDUMP_H
#define CLX_HEXDUMP_H

#include "config.h"
#include <string>
#include <iterator>
#include "convert.h"
#include "hex_convert.h"

namespace clx {
	/* --------------------------------------------------------------------- */
	//  hexdump
	/* --------------------------------------------------------------------- */
	template <class InIter, class OutIter>
	inline OutIter hexdump(InIter first, InIter last, OutIter out) {
		clx::basic_hex_encoder<typename InIter::value_type> f;
		return clx::convert(first, last, out, f);
	}
	
	/* --------------------------------------------------------------------- */
	//  hexdump
	/* --------------------------------------------------------------------- */
	template <class Ch, class Tr>
	inline std::basic_string<Ch, Tr> hexdump(const std::basic_string<Ch, Tr>& src) {
		clx::basic_hex_encoder<Ch, Tr> f;
		return clx::convert(src, f);
	}
	
	/* --------------------------------------------------------------------- */
	//  hexdump
	/* --------------------------------------------------------------------- */
	template <class CharT>
	inline std::basic_string<CharT> hexdump(const CharT* src) {
		std::basic_string<CharT> tmp(src);
		clx::basic_hex_encoder<CharT> f;
		return clx::convert(tmp, f);
	}
}

#endif // CLX_HEXDUMP_H
