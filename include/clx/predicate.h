/* ------------------------------------------------------------------------- */
/*
 *  predicate.h
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
 *  Last-modified: Mon 01 Mar 2010 21:51:15 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_PREDICATE_H
#define CLX_PREDICATE_H

#include "config.h"
#include <locale>
#include <set>
#include "literal.h"

namespace clx {
	/* --------------------------------------------------------------------- */
	//  classified_functor
	/* --------------------------------------------------------------------- */
	class classified_functor {
	public:
		explicit classified_functor(std::ctype_base::mask m, const std::locale& loc = std::locale()) :
			mask_(m), loc_(loc) {}
		
		template <class CharT>
		bool operator()(CharT c) const {
			return std::use_facet<std::ctype<CharT> >(loc_).is(mask_, c);
		}
		
	private:
		const std::ctype_base::mask mask_;
		const std::locale loc_;
	};
	
	/* --------------------------------------------------------------------- */
	//  charset_functor
	/* --------------------------------------------------------------------- */
	template <class CharT>
	class charset_functor {
	public:
		typedef CharT char_type;
		typedef unsigned int size_type;
		
		explicit charset_functor(const char_type* s) : charset_() {
			char_type null_char = LITERAL('\0');
			while (s != NULL && *s != null_char) charset_.insert(*s++);
		}
		
		explicit charset_functor(const std::basic_string<CharT>& s) :
			charset_(s.begin(), s.end()) {}
		
		bool operator()(char_type c) const {
			return charset_.count(c) > 0;
		}
		
	private:
		std::set<char_type> charset_;
	};
	
	/* --------------------------------------------------------------------- */
	//  is_any_of
	/* --------------------------------------------------------------------- */
	template <class CharT>
	inline charset_functor<CharT> is_any_of(const CharT* s) {
		return charset_functor<CharT>(s);
	}
	
	/* --------------------------------------------------------------------- */
	//  is_any_of
	/* --------------------------------------------------------------------- */
	template <class CharT>
	inline charset_functor<CharT> is_any_of(const std::basic_string<CharT>& s) {
		return charset_functor<CharT>(s);
	}
	
	/* --------------------------------------------------------------------- */
	//  is_space
	/* --------------------------------------------------------------------- */
	inline classified_functor is_space(const std::locale& loc = std::locale()) {
		return classified_functor(std::ctype_base::space, loc);
	}
	
	/* --------------------------------------------------------------------- */
	//  is_alnum
	/* --------------------------------------------------------------------- */
	inline classified_functor is_alnum(const std::locale& loc = std::locale()) {
		return classified_functor(std::ctype_base::alnum, loc);
	}
	
	/* --------------------------------------------------------------------- */
	//  is_alpha
	/* --------------------------------------------------------------------- */
	inline classified_functor is_alpha(const std::locale& loc = std::locale()) {
		return classified_functor(std::ctype_base::alpha, loc);
	}
	
	/* --------------------------------------------------------------------- */
	//  is_cntrl
	/* --------------------------------------------------------------------- */
	inline classified_functor is_cntrl(const std::locale& loc = std::locale()) {
		return classified_functor(std::ctype_base::cntrl, loc);
	}
	
	/* --------------------------------------------------------------------- */
	//  is_digit
	/* --------------------------------------------------------------------- */
	inline classified_functor is_digit(const std::locale& loc = std::locale()) {
		return classified_functor(std::ctype_base::digit, loc);
	}
	
	/* --------------------------------------------------------------------- */
	//  is_graph
	/* --------------------------------------------------------------------- */
	inline classified_functor is_graph(const std::locale& loc = std::locale()) {
		return classified_functor(std::ctype_base::graph, loc);
	}
	
	/* --------------------------------------------------------------------- */
	//  is_lower
	/* --------------------------------------------------------------------- */
	inline classified_functor is_lower(const std::locale& loc = std::locale()) {
		return classified_functor(std::ctype_base::lower, loc);
	}
	
	/* --------------------------------------------------------------------- */
	//  is_upper
	/* --------------------------------------------------------------------- */
	inline classified_functor is_upper(const std::locale& loc = std::locale()) {
		return classified_functor(std::ctype_base::upper, loc);
	}
	
	/* --------------------------------------------------------------------- */
	//  is_print
	/* --------------------------------------------------------------------- */
	inline classified_functor is_print(const std::locale& loc = std::locale()) {
		return classified_functor(std::ctype_base::print, loc);
	}
	
	/* --------------------------------------------------------------------- */
	//  is_punct
	/* --------------------------------------------------------------------- */
	inline classified_functor is_punct(const std::locale& loc = std::locale()) {
		return classified_functor(std::ctype_base::punct, loc);
	}
	
	/* --------------------------------------------------------------------- */
	//  is_xdigit
	/* --------------------------------------------------------------------- */
	inline classified_functor is_xdigit(const std::locale& loc = std::locale()) {
		return classified_functor(std::ctype_base::xdigit, loc);
	}
}

#endif // CLX_PREDICATE_H
