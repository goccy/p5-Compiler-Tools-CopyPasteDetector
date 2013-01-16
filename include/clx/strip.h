/* ------------------------------------------------------------------------- */
/*
 *  strip.h
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
 *  Last-modified: Mon 23 Apr 2007 13:45:25 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_STRIP_H
#define CLX_STRIP_H

#include "config.h"
#include <string>
#include <locale>
#include "literal.h"
#include "predicate.h"

namespace clx {
	/* --------------------------------------------------------------------- */
	//  lstrip_if
	/* --------------------------------------------------------------------- */
	template <class CharT, class Traits, class PredicateFunc>
	inline std::basic_string<CharT, Traits>& lstrip_if(
		std::basic_string<CharT, Traits>& s, PredicateFunc f) {
		typename std::basic_string<CharT, Traits>::size_type i;
		for (i = 0; i < s.size(); ++i) {
			if (!f(s.at(i))) break;
		}
		if (i > 0) s.erase(0, i);
		return s;
	}
	
	/* --------------------------------------------------------------------- */
	//  lstrip_copy_if
	/* --------------------------------------------------------------------- */
	template <class CharT, class Traits, class PredicateFunc>
	inline std::basic_string<CharT, Traits> lstrip_copy_if(
		const std::basic_string<CharT, Traits>& s, PredicateFunc f) {
		std::basic_string<CharT, Traits> tmp(s);
		return lstrip_if(tmp, f);
	}
	
	/* --------------------------------------------------------------------- */
	//  rstrip_if
	/* --------------------------------------------------------------------- */
	template <class CharT, class Traits, class PredicateFunc>
	inline std::basic_string<CharT, Traits>& rstrip_if(
		std::basic_string<CharT, Traits>& s, PredicateFunc f) {
		typename std::basic_string<CharT, Traits>::size_type i;
		for (i = s.size(); i > 0; --i) {
			if (!f(s.at(i - 1))) break;
		}
		if (i < s.size()) s.erase(i);
		return s;
	}
	
	/* --------------------------------------------------------------------- */
	//  rstrip_copy_if
	/* --------------------------------------------------------------------- */
	template <class CharT, class Traits, class PredicateFunc>
	inline std::basic_string<CharT, Traits> rstrip_copy_if(
		const std::basic_string<CharT, Traits>& s, PredicateFunc f) {
		std::basic_string<CharT, Traits> tmp(s);
		return rstrip_if(tmp, f);
	}
	
	/* --------------------------------------------------------------------- */
	//  strip_if
	/* --------------------------------------------------------------------- */
	template <class CharT, class Traits, class PredicateFunc>
	inline std::basic_string<CharT, Traits>& strip_if(
		std::basic_string<CharT, Traits>& s, PredicateFunc f) {
		lstrip_if(s, f);
		rstrip_if(s, f);
		return s;
	}
	
	/* --------------------------------------------------------------------- */
	//  strip_copy_if
	/* --------------------------------------------------------------------- */
	template <class CharT, class Traits, class PredicateFunc>
	inline std::basic_string<CharT, Traits> strip_copy_if(
		const std::basic_string<CharT, Traits>& s, PredicateFunc f) {
		std::basic_string<CharT, Traits> tmp(s);
		return strip_if(tmp, f);
	}
	
	/* --------------------------------------------------------------------- */
	//  lstrip
	/* --------------------------------------------------------------------- */
	template <class CharT, class Traits>
	inline std::basic_string<CharT, Traits>& lstrip(
		std::basic_string<CharT, Traits>& s, const std::locale& loc = std::locale()) {
		return lstrip_if(s, is_space(loc));
	}
	
	/* --------------------------------------------------------------------- */
	//  lstrip_copy
	/* --------------------------------------------------------------------- */
	template <class CharT, class Traits>
	inline std::basic_string<CharT, Traits> lstrip_copy(
		const std::basic_string<CharT, Traits>& s, const std::locale& loc = std::locale()) {
		std::basic_string<CharT, Traits> tmp(s);
		return lstrip(tmp, loc);
	}
	
	/* --------------------------------------------------------------------- */
	//  rstrip
	/* --------------------------------------------------------------------- */
	template <class CharT, class Traits>
	inline std::basic_string<CharT, Traits>& rstrip(
		std::basic_string<CharT, Traits>& s, const std::locale& loc = std::locale()) {
		return rstrip_if(s, is_space(loc));
	}
	
	/* --------------------------------------------------------------------- */
	//  rstrip_copy
	/* --------------------------------------------------------------------- */
	template <class CharT, class Traits>
	inline std::basic_string<CharT, Traits> rstrip_copy(
		const std::basic_string<CharT, Traits>& s, const std::locale& loc = std::locale()) {
		std::basic_string<CharT, Traits> tmp(s);
		return rstrip(tmp, loc);
	}
	
	/* --------------------------------------------------------------------- */
	//  strip
	/* --------------------------------------------------------------------- */
	template <class CharT, class Traits>
	inline std::basic_string<CharT, Traits>& strip(
		std::basic_string<CharT, Traits>& s, const std::locale& loc = std::locale()) {
		return strip_if(s, is_space(loc));
	}
	
	/* --------------------------------------------------------------------- */
	//  strip_copy
	/* --------------------------------------------------------------------- */
	template <class CharT, class Traits>
	inline std::basic_string<CharT, Traits> strip_copy(
		const std::basic_string<CharT, Traits>& s, const std::locale& loc = std::locale()) {
		std::basic_string<CharT, Traits> tmp(s);
		return strip(tmp, loc);
	}
	
	/* --------------------------------------------------------------------- */
	//  chop_if
	/* --------------------------------------------------------------------- */
	template <class CharT, class Traits, class PredicateFunc>
	inline std::basic_string<CharT, Traits>& chop_if(
		std::basic_string<CharT, Traits>& s, PredicateFunc f) {
		if (!s.empty() && f(s.at(s.size() - 1))) s.erase(s.size() - 1);
		return s;
	}
	
	/* --------------------------------------------------------------------- */
	//  chop_copy_if
	/* --------------------------------------------------------------------- */
	template <class CharT, class Traits, class PredicateFunc>
	inline std::basic_string<CharT, Traits> chop_copy_if(
		const std::basic_string<CharT, Traits>& s, PredicateFunc f) {
		std::basic_string<CharT, Traits> tmp(s);
		return chop_if(tmp, f);
	}
	
	/* --------------------------------------------------------------------- */
	//  chop
	/* --------------------------------------------------------------------- */
	template <class CharT, class Traits>
	inline std::basic_string<CharT, Traits>& chop(std::basic_string<CharT, Traits>& s) {
		if (!s.empty()) s.erase(s.size() - 1);
		return s;
	}
	
	/* --------------------------------------------------------------------- */
	//  chop_copy
	/* --------------------------------------------------------------------- */
	template <class CharT, class Traits>
	inline std::basic_string<CharT, Traits> chop_copy(const std::basic_string<CharT, Traits>& s) {
		std::basic_string<CharT, Traits> tmp(s);
		return chop(tmp);
	}
	
	/* --------------------------------------------------------------------- */
	//  chomp
	/* --------------------------------------------------------------------- */
	template <class CharT, class Traits>
	inline std::basic_string<CharT, Traits>& chomp(std::basic_string<CharT, Traits>& s) {
		const CharT* lf = LITERAL("\n");
		const CharT* cr = LITERAL("\r");
		chop_if(s, is_any_of(lf));
		chop_if(s, is_any_of(cr));
		return s;
	}
	
	/* --------------------------------------------------------------------- */
	//  chomp_copy
	/* --------------------------------------------------------------------- */
	template <class CharT, class Traits>
	inline std::basic_string<CharT, Traits>& chomp_copy(const std::basic_string<CharT, Traits>& s) {
		std::basic_string<CharT, Traits> tmp(s);
		return chomp(tmp);
	}
}

#endif // CLX_STRIP_H
