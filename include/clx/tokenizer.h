/* ------------------------------------------------------------------------- */
/*
 *  tokenizer.h
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
#ifndef CLX_TOKENIZER_H
#define CLX_TOKENIZER_H

#include "config.h"
#include <string>
#include <vector>
#include "config.h"
#include "tokenizer_func.h"
#include "container_generator.h"
#include "container_accessor.h"

namespace clx {
	/* --------------------------------------------------------------------- */
	//  basic_tokenizer
	/* --------------------------------------------------------------------- */
	template <
		class TokenizerFunc,
		class Type = std::basic_string<char>,
		class Container = std::vector<Type>
	>
	class basic_tokenizer :
		private container_generator<Container>,
		public random_accessor<Container> {
	private:
		typedef container_generator<Container> generator;
		typedef random_accessor<Container> accessor;
	public:
		typedef TokenizerFunc functor;
		typedef typename functor::char_type char_type;
		typedef std::basic_string<char_type> string_type;
		typedef Type value_type;
		typedef Container container;
		
		// constructor and destructor
		basic_tokenizer(const basic_tokenizer& cp) :
			generator(cp.member), accessor(this->member), f_(cp.f_) {}
		
		explicit basic_tokenizer(functor f = functor()) :
			generator(), accessor(this->member), f_(f) {}
		
		template <class InIter>
		basic_tokenizer(InIter first, InIter last, functor f = functor()) :
			generator(), accessor(this->member), f_(f) { this->assign(first, last); }
		
		explicit basic_tokenizer(const string_type& s, functor f = functor()) :
			generator(), accessor(this->member), f_(f) { this->assign(s); }
		
		explicit basic_tokenizer(const char_type* s, functor f = functor()) :
			generator(), accessor(this->member), f_(f) { this->assign(s); }
		
		virtual ~basic_tokenizer() throw() {}
		
		basic_tokenizer& operator=(const basic_tokenizer& cp) {
			this->member = cp.member;
			f_ = cp.f_;
			return *this;
		}
		
		//operator
		void reset() {
			this->member.clear();
			f_.reset();
		}
		
		template <class InIter>
		basic_tokenizer& assign(InIter first, InIter last) {
			value_type token;
			
			this->reset();
			while (f_(first, last, token)) {
				this->member.insert(this->member.end(), token);
				if (first == last) break;
				token = value_type();
			}
			return *this;
		}
		
		basic_tokenizer& assign(const string_type& s) {
			if (s.empty()) return *this;
			return this->assign(s.begin(), s.end());
		}
		
		basic_tokenizer& assign(const char_type* s) {
			string_type tmp(s);
			return this->assign(tmp);
		}
		
		template <class InIter>
		basic_tokenizer& operator()(InIter first, InIter last) {
			return this->assign(first, last);
		}
		
		basic_tokenizer& operator()(const string_type& s) { return this->assign(s); }
		basic_tokenizer& operator()(const char_type* s) { return this->assign(s); }
		
	private:
		functor f_;
	};
	
	typedef basic_tokenizer<char_separator<char>, std::basic_string<char> > tokenizer;
	typedef basic_tokenizer<escape_separator<char>, std::basic_string<char> > csv;
	typedef basic_tokenizer<format_separator<char>, std::basic_string<char> > strftokenizer;
#ifdef CLX_USE_WCHAR
	typedef basic_tokenizer<char_separator<wchar_t>, std::basic_string<wchar_t> > wtokenizer;
	typedef basic_tokenizer<escape_separator<wchar_t>, std::basic_string<wchar_t> > wcsv;
	typedef basic_tokenizer<format_separator<wchar_t>, std::basic_string<wchar_t> > wstrftokenizer;
#endif // CLX_USE_WCHAR
	
	/* --------------------------------------------------------------------- */
	/*
	 *  create_tokenizer
	 *
	 *  The function helps for creating a tokenizer object. This
	 *  function is mainly used when using the tokenizer as functor
	 *  class.
	 */
	/* --------------------------------------------------------------------- */
	template <class Type, class TokenizerFunc>
	inline basic_tokenizer<TokenizerFunc, Type> create_tokenizer(TokenizerFunc f) {
		return basic_tokenizer<TokenizerFunc, Type>(f);
	}
}

#endif // CLX_TOKENIZER_H
