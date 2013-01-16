/* ------------------------------------------------------------------------- */
/*
 *  replace.h
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
 *  Last-modified: Thu 26 Apr 2007 19:25:08 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_REPLACE_H
#define CLX_REPLACE_H

#include "config.h"
#include <string>

namespace clx {
	/* --------------------------------------------------------------------- */
	//  replace
	/* --------------------------------------------------------------------- */
	template <class CharT, class Traits>
	inline std::basic_string<CharT, Traits>& replace(std::basic_string<CharT, Traits>& s,
		const std::basic_string<CharT, Traits>& sch,
		const std::basic_string<CharT, Traits>& rep,
		unsigned int nth = 1) {
		if (s.empty()) return s;
		
		unsigned int i = 0;
		typename std::basic_string<CharT, Traits>::size_type pos = 0;
		while ((pos = s.find(sch, pos)) != std::basic_string<CharT, Traits>::npos) {
			if (++i == nth) {
				s.replace(pos, sch.length(), rep);
				break;
			}
			pos += sch.length();
		}
		return s;
	}
	
	/* --------------------------------------------------------------------- */
	//  replace
	/* --------------------------------------------------------------------- */
	template <class CharT, class Traits>
	inline std::basic_string<CharT, Traits>& replace(std::basic_string<CharT, Traits>& s,
		const CharT* sch, const CharT* rep, unsigned int nth = 1) {
		std::basic_string<CharT, Traits> sch_tmp(sch);
		std::basic_string<CharT, Traits> rep_tmp(rep);
		return replace(s, sch_tmp, rep_tmp, nth);
	}
	
	/* --------------------------------------------------------------------- */
	//  replace_copy
	/* --------------------------------------------------------------------- */
	template <class CharT, class Traits>
	inline std::basic_string<CharT, Traits> replace_copy(
		const std::basic_string<CharT, Traits>& s,
		const std::basic_string<CharT, Traits>& sch,
		const std::basic_string<CharT, Traits>& rep,
		unsigned int nth = 1) {
		std::basic_string<CharT, Traits> tmp(s);
		return replace(tmp, sch, rep, nth);
	}
	
	/* --------------------------------------------------------------------- */
	//  replace_copy
	/* --------------------------------------------------------------------- */
	template <class CharT, class Traits>
	inline std::basic_string<CharT, Traits> replace_copy(
		const std::basic_string<CharT, Traits>& s,
		const CharT* sch, const CharT* rep, unsigned int nth = 1) {
		std::basic_string<CharT, Traits> tmp(s);
		return replace(tmp, sch, rep, nth);
	}
	
	/* --------------------------------------------------------------------- */
	//  replace_all
	/* --------------------------------------------------------------------- */
	template <class CharT, class Traits>
	inline std::basic_string<CharT, Traits>& replace_all(std::basic_string<CharT, Traits>& s,
		const std::basic_string<CharT, Traits>& sch,
		const std::basic_string<CharT, Traits>& rep) {
		if (s.empty()) return s;
		
		typename std::basic_string<CharT, Traits>::size_type pos = 0;
		while ((pos = s.find(sch, pos)) != std::basic_string<CharT, Traits>::npos) {
			s.replace(pos, sch.length(), rep);
			pos += rep.length();
		}
		return s;
	}
	
	/* --------------------------------------------------------------------- */
	//  replace_all
	/* --------------------------------------------------------------------- */
	template <class CharT, class Traits>
	inline std::basic_string<CharT, Traits>& replace_all(std::basic_string<CharT, Traits>& s,
		const CharT* sch, const CharT* rep) {
		std::basic_string<CharT, Traits> sch_tmp(sch);
		std::basic_string<CharT, Traits> rep_tmp(rep);
		return replace_all(s, sch_tmp, rep_tmp);
	}
	
	/* --------------------------------------------------------------------- */
	//  replace_all_copy
	/* --------------------------------------------------------------------- */
	template <class CharT, class Traits>
	inline std::basic_string<CharT, Traits> replace_all_copy(
		const std::basic_string<CharT, Traits>& s,
		const std::basic_string<CharT, Traits>& sch,
		const std::basic_string<CharT, Traits>& rep) {
		std::basic_string<CharT, Traits> tmp(s);
		return replace_all(tmp, sch, rep);
	}
	
	/* --------------------------------------------------------------------- */
	//  replace_all_copy
	/* --------------------------------------------------------------------- */
	template <class CharT, class Traits>
	inline std::basic_string<CharT, Traits> replace_all_copy(
		const std::basic_string<CharT, Traits>& s,
		const CharT* sch, const CharT* rep) {
		std::basic_string<CharT, Traits> tmp(s);
		return replace_all(tmp, sch, rep);
	}
}

#endif // CLX_REPLACE_H
