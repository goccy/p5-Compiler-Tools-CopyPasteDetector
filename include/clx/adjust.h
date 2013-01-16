/* ------------------------------------------------------------------------- */
/*
 *  adjust.h
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
 *  Last-modified: Thu 26 Apr 2007 19:55:04 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_ADJUST_H
#define CLX_ADJUST_H

#include "config.h"
#include <string>
#include "literal.h"

namespace clx {
	/* --------------------------------------------------------------------- */
	//  ljust
	/* --------------------------------------------------------------------- */
	template <class CharT, class Traits>
	inline std::basic_string<CharT, Traits>& ljust(std::basic_string<CharT, Traits>& s,
		unsigned int n, CharT c = LITERAL(' ')) {
		if (s.length() < n) s.append(n - s.length(), c);
		return s;
	}
	
	/* --------------------------------------------------------------------- */
	//  ljust_copy
	/* --------------------------------------------------------------------- */
	template <class CharT, class Traits>
	inline std::basic_string<CharT, Traits> ljust_copy(const std::basic_string<CharT, Traits>& s,
		unsigned int n, CharT c = LITERAL(' ')) {
		std::basic_string<CharT, Traits> tmp(s);
		return ljust(tmp, n, c);
	}
	
	/* --------------------------------------------------------------------- */
	//  rjust
	/* --------------------------------------------------------------------- */
	template <class CharT, class Traits>
	inline std::basic_string<CharT, Traits>& rjust(std::basic_string<CharT, Traits>& s,
		unsigned int n, CharT c = LITERAL(' ')) {
		if (s.length() < n) s.insert(s.begin(), n - s.length(), c);
		return s;
	}
	
	/* --------------------------------------------------------------------- */
	//  rjust_copy
	/* --------------------------------------------------------------------- */
	template <class CharT, class Traits>
	inline std::basic_string<CharT, Traits> rjust_copy(const std::basic_string<CharT, Traits>& s,
		unsigned int n, CharT c = LITERAL(' ')) {
		std::basic_string<CharT, Traits> tmp(s);
		return rjust(tmp, n, c);
	}
	
	/* --------------------------------------------------------------------- */
	//  center
	/* --------------------------------------------------------------------- */
	template <class CharT, class Traits>
	inline std::basic_string<CharT, Traits>& center(std::basic_string<CharT, Traits>& s,
		unsigned int n, CharT c = LITERAL(' ')) {
		if (s.length() < n) {
			unsigned int quo = (n - s.length()) / 2;
			unsigned int odd = (n - s.length()) % 2;
			ljust(s, quo + s.length(), c);
			if (odd != 0) ++quo;
			rjust(s, quo + s.length(), c);
		}
		return s;
	}
	
	/* --------------------------------------------------------------------- */
	//  center_copy
	/* --------------------------------------------------------------------- */
	template <class CharT, class Traits>
	inline std::basic_string<CharT, Traits> center_copy(const std::basic_string<CharT, Traits>& s,
		unsigned int n, CharT c = LITERAL(' ')) {
		std::basic_string<CharT, Traits> tmp(s);
		return center(tmp, n, c);
	}
}

#endif // CLX_ADJUST_H
