/* ------------------------------------------------------------------------- */
/*
 *  case_conv.h
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
 *  Last-modified: Sat 17 Mar 2007 20:07:23 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_CASE_CONV_H
#define CLX_CASE_CONV_H

#include "config.h"
#include <string>
#include <locale>
#include "predicate.h"

namespace clx {
	/* --------------------------------------------------------------------- */
	//  upcase
	/* --------------------------------------------------------------------- */
	template <class CharT, class Traits>
	inline std::basic_string<CharT, Traits>& upcase(
		std::basic_string<CharT, Traits>& s, const std::locale& loc = std::locale()) {
		if (s.empty()) return s;
		
		typename std::basic_string<CharT, Traits>::iterator p;
		for (p = s.begin(); p != s.end(); ++p) {
			*p = std::use_facet<std::ctype<CharT> >(loc).toupper(*p);
		}
		return s;
	}
	
	/* --------------------------------------------------------------------- */
	//  upcase_copy
	/* --------------------------------------------------------------------- */
	template <class CharT, class Traits>
	inline std::basic_string<CharT, Traits> upcase_copy(
		const std::basic_string<CharT, Traits>& s, const std::locale& loc = std::locale()) {
		std::basic_string<CharT, Traits> tmp(s);
		return upcase(tmp, loc);
	}
	
	/* --------------------------------------------------------------------- */
	//  upcase_if
	/* --------------------------------------------------------------------- */
	template <class CharT, class Traits, class PredicateFunc>
	inline std::basic_string<CharT, Traits>& upcase_if(std::basic_string<CharT, Traits>& s,
		PredicateFunc f, const std::locale& loc = std::locale()) {
		if (s.empty()) return s;
		
		typename std::basic_string<CharT, Traits>::iterator p;
		for (p = s.begin(); p != s.end(); ++p) {
			if (f(*p)) *p = std::use_facet<std::ctype<CharT> >(loc).toupper(*p);
		}
		return s;
	}
	
	/* --------------------------------------------------------------------- */
	//  upcase_copy_if
	/* --------------------------------------------------------------------- */
	template <class CharT, class Traits, class PredicateFunc>
	inline std::basic_string<CharT, Traits> upcase_copy_if(const std::basic_string<CharT, Traits>& s,
		PredicateFunc f, const std::locale& loc = std::locale()) {
		std::basic_string<CharT, Traits> tmp(s);
		return upcase_if(tmp, f, loc);
	}
	
	/* --------------------------------------------------------------------- */
	//  downcase
	/* --------------------------------------------------------------------- */
	template <class CharT, class Traits>
	inline std::basic_string<CharT, Traits>& downcase(
		std::basic_string<CharT, Traits>& s, const std::locale& loc = std::locale()) {
		if (s.empty()) return s;
		
		typename std::basic_string<CharT, Traits>::iterator p;
		for (p = s.begin(); p != s.end(); ++p) {
			*p = std::use_facet<std::ctype<CharT> >(loc).tolower(*p);
		}
		return s;
	}
	
	/* --------------------------------------------------------------------- */
	//  downcase_copy
	/* --------------------------------------------------------------------- */
	template <class CharT, class Traits>
	inline std::basic_string<CharT, Traits> downcase_copy(
		const std::basic_string<CharT, Traits>& s, const std::locale& loc = std::locale()) {
		std::basic_string<CharT, Traits> tmp(s);
		return downcase(tmp);
	}
	
	/* --------------------------------------------------------------------- */
	//  downcase_if
	/* --------------------------------------------------------------------- */
	template <class CharT, class Traits, class PredicateFunc>
	inline std::basic_string<CharT, Traits>& downcase_if(std::basic_string<CharT, Traits>& s,
		PredicateFunc f, const std::locale& loc = std::locale()) {
		if (s.empty()) return s;
		
		typename std::basic_string<CharT, Traits>::iterator p;
		for (p = s.begin(); p != s.end(); ++p) {
			if (f(*p)) *p = std::use_facet<std::ctype<CharT> >(loc).tolower(*p);
		}
		return s;
	}
	
	/* --------------------------------------------------------------------- */
	//  downcase_copy_if
	/* --------------------------------------------------------------------- */
	template <class CharT, class Traits, class PredicateFunc>
	inline std::basic_string<CharT, Traits> downcase_copy_if(const std::basic_string<CharT, Traits>& s,
		PredicateFunc f, const std::locale& loc = std::locale()) {
		std::basic_string<CharT, Traits> tmp(s);
		return downcase_if(tmp, f, loc);
	}
	
	/* --------------------------------------------------------------------- */
	//  swapcase
	/* --------------------------------------------------------------------- */
	template <class CharT, class Traits>
	inline std::basic_string<CharT, Traits>& swapcase(
		std::basic_string<CharT, Traits>& s, const std::locale& loc = std::locale()) {
		if (s.empty()) return s;
		
		typename std::basic_string<CharT, Traits>::iterator p;
		for (p = s.begin(); p != s.end(); ++p) {
			if (std::isupper(*p, loc)) *p = std::use_facet<std::ctype<CharT> >(loc).tolower(*p);
			else if (std::islower(*p, loc)) *p = std::use_facet<std::ctype<CharT> >(loc).toupper(*p);
		}
		return s;
	}
	
	/* --------------------------------------------------------------------- */
	//  swapcase_copy
	/* --------------------------------------------------------------------- */
	template <class CharT, class Traits>
	inline std::basic_string<CharT, Traits> swapcase_copy(
		const std::basic_string<CharT, Traits>& s, const std::locale& loc = std::locale()) {
		std::basic_string<CharT, Traits> tmp(s);
		return swapcase(tmp, loc);
	}
	
	/* --------------------------------------------------------------------- */
	//  swapcase_if
	/* --------------------------------------------------------------------- */
	template <class CharT, class Traits, class PredicateFunc>
	inline std::basic_string<CharT, Traits>& swapcase_if(std::basic_string<CharT, Traits>& s,
		PredicateFunc f, const std::locale& loc = std::locale()) {
		typename std::basic_string<CharT, Traits>::iterator p;
		for (p = s.begin(); p != s.end(); ++p) {
			if (std::isupper(*p, loc) && f(*p)) *p = std::use_facet<std::ctype<CharT> >(loc).tolower(*p);
			else if (std::islower(*p, loc) && f(*p)) *p = std::use_facet<std::ctype<CharT> >(loc).toupper(*p);
		}
		return s;
	}
	
	/* --------------------------------------------------------------------- */
	//  swapcase_copy_if
	/* --------------------------------------------------------------------- */
	template <class CharT, class Traits, class PredicateFunc>
	inline std::basic_string<CharT, Traits> swapcase_copy_if(const std::basic_string<CharT, Traits>& s,
		PredicateFunc f, const std::locale& loc = std::locale()) {
		std::basic_string<CharT, Traits> tmp(s);
		return swapcase_if(s, f, loc);
	}
	
	/* --------------------------------------------------------------------- */
	//  capitalize
	/* --------------------------------------------------------------------- */
	template <class CharT, class Traits>
	inline std::basic_string<CharT, Traits>& capitalize(
		std::basic_string<CharT, Traits>& s, const std::locale& loc = std::locale()) {
		if (s.empty()) return s;
		
		typename std::basic_string<CharT, Traits>::iterator first = s.begin();
		if (first != s.end()) *first = std::use_facet<std::ctype<CharT> >(loc).toupper(*first);
		return s;
	}
	
	/* --------------------------------------------------------------------- */
	//  capitalize_copy
	/* --------------------------------------------------------------------- */
	template <class CharT, class Traits>
	inline std::basic_string<CharT, Traits> capitalize_copy(
		const std::basic_string<CharT, Traits>& s, const std::locale& loc = std::locale()) {
		std::basic_string<CharT, Traits> tmp(s);
		return capitalize(tmp);
	}
	
	/* --------------------------------------------------------------------- */
	//  capitalize_if
	/* --------------------------------------------------------------------- */
	template <class CharT, class Traits, class PredicateFunc>
	inline std::basic_string<CharT, Traits>& capitalize(std::basic_string<CharT, Traits>& s,
		PredicateFunc f, const std::locale& loc = std::locale()) {
		typename std::basic_string<CharT, Traits>::iterator first = s.begin();
		if (first != s.end() && f(*first)) *first = std::use_facet<std::ctype<CharT> >(loc).toupper(*first);
		return s;
	}
	
	/* --------------------------------------------------------------------- */
	//  capitalize_copy_if
	/* --------------------------------------------------------------------- */
	template <class CharT, class Traits, class PredicateFunc>
	inline std::basic_string<CharT, Traits> capitalize_copy(const std::basic_string<CharT, Traits>& s,
		PredicateFunc f, const std::locale& loc = std::locale()) {
		std::basic_string<CharT, Traits> tmp(s);
		return capitalize_if(tmp, f);
	}
}

#endif // CLX_CASE_CONV_H
