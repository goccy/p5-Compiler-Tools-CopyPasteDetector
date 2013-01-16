/* ------------------------------------------------------------------------- */
/*
 *  convert.h
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
 *  Last-modified: Mon 01 Mar 2010 19:04:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_CONVERT_H
#define CLX_CONVERT_H

#include "config.h"
#include <string>
#include <iterator>
#include "convert_policy.h"

namespace clx {
	/* --------------------------------------------------------------------- */
	//  convert
	/* --------------------------------------------------------------------- */
	template <class InIter, class OutIter, class ConvPolicy>
	inline OutIter convert(InIter first, InIter last, OutIter result, ConvPolicy f) {
		result = f.reset(result);
		for (; first != last; ++first) result = f(*first, result);
		result = f.finish(result);
		return result;
	}
	
	/* --------------------------------------------------------------------- */
	//  convert
	/* --------------------------------------------------------------------- */
	template <class Ch, class Tr, class ConvPolicy>
	inline std::basic_string<Ch, Tr> convert(const std::basic_string<Ch, Tr>& src, ConvPolicy f) {
		std::basic_string<Ch, Tr> dest;
		std::insert_iterator<std::basic_string<Ch, Tr> > out(dest, dest.end());
		clx::convert(src.begin(), src.end(), out, f);
		return dest;
	}
	
	/* --------------------------------------------------------------------- */
	//  convert
	/* --------------------------------------------------------------------- */
	template <class CharT, class ConvPolicy>
	inline std::basic_string<CharT> convert(const CharT* src, ConvPolicy f) {
		std::basic_string<CharT> tmp(src);
		std::basic_string<CharT> dest;
		std::insert_iterator<std::basic_string<CharT> > out(dest, dest.end());
		clx::convert(tmp.begin(), tmp.end(), out, f);
		return dest;
	}
} // namespace clx

#endif // CLX_CONVERT_H
