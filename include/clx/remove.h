/* ------------------------------------------------------------------------- */
/*
 *  remove.h
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
 *  Last-modified: Thu 26 Apr 2007 17:55:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_REMOVE_H
#define CLX_REMOVE_H

#include "config.h"
#include <string>
#include <algorithm>
#include <functional>
#include "predicate.h"

namespace clx {
	/* --------------------------------------------------------------------- */
	//  remove_if
	/* --------------------------------------------------------------------- */
	template <class CharT, class Traits, class PredicateFunc>
	inline std::basic_string<CharT, Traits>& remove_if(
		std::basic_string<CharT, Traits>& s, PredicateFunc f) {
		if (s.empty()) return s;
		
		typename std::basic_string<CharT, Traits>::iterator last;
		last = std::remove_if(s.begin(), s.end(), f);
		if (last != s.end()) s.erase(last, s.end());
		return s;
	}
	
	/* --------------------------------------------------------------------- */
	//  remove_copy_if
	/* --------------------------------------------------------------------- */
	template <class CharT, class Traits, class PredicateFunc>
	inline std::basic_string<CharT, Traits> remove_copy_if(
		const std::basic_string<CharT, Traits>& s, PredicateFunc f) {
		std::basic_string<CharT, Traits> tmp(s);
		return remove_if(tmp, f);
	}
	
	/* --------------------------------------------------------------------- */
	//  remove
	/* --------------------------------------------------------------------- */
	template <class CharT, class Traits>
	inline std::basic_string<CharT, Traits>& remove(
		std::basic_string<CharT, Traits>& s, CharT c) {
		return remove_if(s, std::bind2nd(std::equal_to<CharT>(), c));
	}
	
	/* --------------------------------------------------------------------- */
	//  remove_copy
	/* --------------------------------------------------------------------- */
	template <class CharT, class Traits>
	inline std::basic_string<CharT, Traits> remove_copy(
		const std::basic_string<CharT, Traits>& s, CharT c) {
		std::basic_string<CharT, Traits> tmp(s);
		return remove(tmp, c);
	}
	
	/* --------------------------------------------------------------------- */
	//  unique
	/* --------------------------------------------------------------------- */
	template <class CharT, class Traits>
	inline std::basic_string<CharT, Traits>& unique(std::basic_string<CharT, Traits>& s) {
		if (s.empty()) return s;
		
		typename std::basic_string<CharT, Traits>::iterator last;
		last = std::unique(s.begin(), s.end());
		if (last != s.end()) s.erase(last, s.end());
		return s;
	}
	
	/* --------------------------------------------------------------------- */
	//  unique_copy
	/* --------------------------------------------------------------------- */
	template <class CharT, class Traits>
	inline std::basic_string<CharT, Traits> unique_copy(const std::basic_string<CharT, Traits>& s) {
		std::basic_string<CharT, Traits> tmp(s);
		return unique(tmp);
	}
	
	/* --------------------------------------------------------------------- */
	//  squeeze_if
	/* --------------------------------------------------------------------- */
	template <class CharT, class Traits, class PredicateFunc>
	inline std::basic_string<CharT, Traits>& squeeze_if(
		std::basic_string<CharT, Traits>& s, PredicateFunc f) {
		if (s.empty()) return s;
		
		bool x = false;
		typename std::basic_string<CharT, Traits>::iterator pos = s.begin();
		while (pos != s.end()) {
			if (f(*pos)) {
				if (x) pos = s.erase(pos);
				else {
					x = true;
					++pos;
				}
			}
			else {
				x = false;
				++pos;
			}
		}
		
		return s;
	}
	
	/* --------------------------------------------------------------------- */
	//  squeeze_copy_if
	/* --------------------------------------------------------------------- */
	template <class CharT, class Traits, class PredicateFunc>
	inline std::basic_string<CharT, Traits> squeeze_copy_if(
		const std::basic_string<CharT, Traits>& s, PredicateFunc f) {
		std::basic_string<CharT, Traits> tmp(s);
		return squeeze_if(tmp, f);
	}
	
	/* --------------------------------------------------------------------- */
	//  squeeze
	/* --------------------------------------------------------------------- */
	template <class CharT, class Traits>
	inline std::basic_string<CharT, Traits>& squeeze(
		std::basic_string<CharT, Traits>& s, CharT c) {
		return squeeze_if(s, std::bind2nd(std::equal_to<CharT>(), c));
	}
	
	/* --------------------------------------------------------------------- */
	//  squeeze_copy
	/* --------------------------------------------------------------------- */
	template <class CharT, class Traits>
	inline std::basic_string<CharT, Traits> squeeze_copy(
		const std::basic_string<CharT, Traits> s, CharT c) {
		std::basic_string<CharT, Traits> tmp(s);
		return squeeze(tmp, c);
	}
}
#endif // CLX_REMOVE_H
