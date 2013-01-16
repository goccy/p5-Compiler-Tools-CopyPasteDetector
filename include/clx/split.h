/* ------------------------------------------------------------------------- */
/*
 *  split.h
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
 *  Last-modified: Fri 19 Jun 2009 01:24:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_SPLIT_H
#define CLX_SPLIT_H

#include "config.h"
#include <iterator>
#include "predicate.h"
#include "lexical_cast.h"

namespace clx {
	/* --------------------------------------------------------------------- */
	//  split_if
	/* --------------------------------------------------------------------- */
	template <class CharT, class Traits, class Container, class PredicateFunc>
	inline Container& split_if(const std::basic_string<CharT, Traits>& s, Container& result,
		PredicateFunc f, bool emtok = false) {
		typedef typename Container::value_type value_type;
		typedef typename std::basic_string<CharT, Traits>::const_iterator const_iterator;
		
		if (s.empty()) return result;
		std::basic_string<CharT, Traits> tmp;
		std::insert_iterator<Container> it(result, result.end());
		for (const_iterator pos = s.begin(); pos != s.end(); ++pos) {
			if (!f(*pos)) tmp += *pos;
			else if (emtok || !tmp.empty()) {
				*it = lexical_cast<value_type>(tmp);
				++it;
				tmp.erase();
			}
		}
		
		if (!tmp.empty()) {
			*it = lexical_cast<value_type>(tmp);
			++it;
		}
		
		return result;
	}
	
	/* --------------------------------------------------------------------- */
	//  split
	/* --------------------------------------------------------------------- */
	template <class CharT, class Traits, class Container>
	inline Container& split(const std::basic_string<CharT, Traits>& s, Container& result,
		bool emtok = false, const std::locale& loc = std::locale()) {
		return split_if(s, result, is_space(loc), emtok);
	}
	
	/* --------------------------------------------------------------------- */
	//  join
	/* --------------------------------------------------------------------- */
	template <class CharT, class Traits, class Container>
	inline std::basic_string<CharT, Traits>& join(const Container& v,
		std::basic_string<CharT, Traits>& result, const std::basic_string<CharT, Traits>& delim) {
		if (v.empty()) return result;
		typename Container::const_iterator pos = v.begin();
		if (pos != v.end()) result += *pos++;
		while (pos != v.end()) {
			result += delim;
			result += *pos;
			++pos;
		}
		return result;
	}
	
	/* --------------------------------------------------------------------- */
	//  join
	/* --------------------------------------------------------------------- */
	template <class CharT, class Traits, class Container>
	inline std::basic_string<CharT, Traits>& join(const Container& v,
		std::basic_string<CharT, Traits>& result, const CharT* delim) {
		std::basic_string<CharT, Traits> tmp(delim);
		return join(v, result, tmp);
	}
}

#endif // CLX_SPLIT_H
