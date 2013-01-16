/* ------------------------------------------------------------------------- */
/*
 *  tokenmap.h
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
 *  Last-modified: Sat 07 Nov 2009 05:37:03 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_TOKENMAP_H
#define CLX_TOKENMAP_H

#include "config.h"
#include <string>
#include <map>
#include "config.h"
#include "tokenizer_func.h"
#include "container_generator.h"
#include "container_accessor.h"

namespace clx {
	/* --------------------------------------------------------------------- */
	//  basic_tokenmap
	/* --------------------------------------------------------------------- */
	template <
		class TokenizerFunc,
		class KeyT = char,
		class ValueT = std::basic_string<char>,
		class Container = std::map<KeyT, ValueT>
	>
	class basic_tokenmap :
		private comparable_container_generator<Container>,
		public map_accessor<Container> {
	private:
		typedef comparable_container_generator<Container> generator;
		typedef map_accessor<Container> accessor;
	public:
		typedef TokenizerFunc functor;
		typedef typename functor::char_type char_type;
		typedef std::basic_string<char_type> string_type;
		typedef KeyT key_type;
		typedef ValueT value_type;
		typedef Container container;
		
		// constructor and destructor
		basic_tokenmap(const basic_tokenmap& cp) :
			generator(cp.member), accessor(this->member), f_(cp.f_) {}
		
		explicit basic_tokenmap(functor f = functor()) :
			generator(), accessor(this->member), f_(f) {}
		
		template <class InIter>
		basic_tokenmap(InIter first, InIter last, functor f = functor()) :
			generator(), accessor(this->member), f_(f) { this->assign(first, last); }
		
		explicit basic_tokenmap(const string_type& s, functor f = functor()) :
			generator(), accessor(this->member), f_(f) { this->assign(s); }
		
		explicit basic_tokenmap(const char_type* s, functor f = functor()) :
			generator(), accessor(this->member), f_(f) { this->assign(s); }
		
		virtual ~basic_tokenmap() throw() {}
		
		basic_tokenmap& operator=(const basic_tokenmap& cp) {
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
		basic_tokenmap& assign(InIter first, InIter last) {
			key_type key;
			value_type val;
			
			this->reset();
			while (f_(first, last, key, val)) {
				this->member.insert(std::make_pair(key, val));
				if (first == last) break;
				key = key_type();
				val = value_type();
			}
			return *this;
		}
		
		basic_tokenmap& assign(const string_type& s) {
			if (s.empty()) return *this;
			return this->assign(s.begin(), s.end());
		}
		
		basic_tokenmap& assign(const char_type* s) {
			string_type tmp(s);
			return this->assign(tmp);
		}
		
		template <class InIter>
		basic_tokenmap& operator()(InIter first, InIter last) {
			return this->assign(first, last);
		}
		
		basic_tokenmap& operator()(const string_type& s) { return this->assign(s); }
		basic_tokenmap& operator()(const char_type* s) { return this->assign(s); }
		
	private:
		functor f_;
	};
	
	typedef basic_tokenmap<format_separator<char>, char, std::basic_string<char> > tokenmap;
#ifdef CLX_USE_WCHAR
	typedef basic_tokenmap<format_separator<wchar_t>, wchar_t, std::basic_string<wchar_t> > wtokenmap;
#endif // CLX_USE_WCHAR
	
	/* --------------------------------------------------------------------- */
	//  create_tokenmap
	/* --------------------------------------------------------------------- */
	template <class KeyT, class ValueT, class TokenizerFunc>
	inline basic_tokenmap<TokenizerFunc, KeyT, ValueT> create_tokenmap(TokenizerFunc f) {
		return basic_tokenmap<TokenizerFunc, KeyT, ValueT>(f);
	}
}

#endif // CLX_TOKENMAP_H
