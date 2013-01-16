/* ------------------------------------------------------------------------- */
/*
 *  argument.h
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
 *  Last-modified: Fri 17 Apr 2009 05:04:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_ARGUMENT_H
#define CLX_ARGUMENT_H

#include "config.h"
#include <string>
#include <vector>
#include <map>
#include "literal.h"
#include "lexical_cast.h"
#include "tokenizer.h"

namespace clx {
	/* --------------------------------------------------------------------- */
	/*
	 *  basic_argument
	 *
	 *  BNF description of the argument is as follow:
	 *  <program> [<nokey_param>]* (['-'<key>] | ['-'+<key> [<param>]*])*
	 */
	/* --------------------------------------------------------------------- */
	template <
		class CharT,
		class Traits = std::char_traits<CharT>
	>
	class basic_argument {
	public:
		typedef CharT char_type;
		typedef size_t size_type;
		typedef std::basic_string<CharT, Traits> string_type;
		typedef std::vector<string_type> container;
		typedef std::map<string_type, container> map_type;
		typedef typename map_type::key_type key_type;
		typedef typename map_type::mapped_type param_list;
		
		// constructor and destructor
		explicit basic_argument(char_type c = LITERAL('-')) :
			identifier_(c), program_(), head_(), v_() {}
		
		explicit basic_argument(int argc, char_type* argv[], char_type c = LITERAL('-')) :
			identifier_(c), program_(), head_(),v_() {
			this->assign(argc, argv);
		}
		
		virtual ~basic_argument() {}
		
		// operator
		basic_argument& assign(int argc, char_type* argv[]) {
			static const char_type zero = LITERAL('0');
			static const char_type nine = LITERAL('9');
			
			program_ = argv[0];
			string_type key;
			bool eoh = false; // end of head parameter
			for (int i = 1; i < argc; ++i) {
				string_type tmp(argv[i]);
				if (tmp.at(0) == identifier_ && tmp.size() > 1 &&
					//!(tmp.at(1) >= zero && tmp.at(1) <= nine)) {
					!(tmp.at(1) >= zero && tmp.at(1) <= nine)) {
					eoh = true;
					if (tmp.at(1) != identifier_) {
						for (size_type j = 1; j < tmp.size(); ++j) {
							key = tmp.at(j);
							this->set_param(key, string_type());
						}
					}
					else {
						key = tmp.substr(tmp.find_first_not_of(identifier_));
						this->set_param(key, string_type());
					}
				}
				else { // add a parameter to the entry of "key"
					if (eoh && key.empty()) break;
					if (key.empty()) head_.push_back(tmp);
					else this->set_param(key, tmp);
				}
			}
			return *this;
		}
		
		// access
		string_type program() { return program_; }
		
		param_list& head() { return head_; }
		
		param_list& operator[](const string_type& x) { return v_[x]; }
		
		param_list& operator[](const char_type* x) {
			string_type tmp(x);
			return v_[tmp];
		}
		
		param_list& operator[](char_type x) {
			string_type tmp(1, x);
			return v_[tmp];
		}
		
		bool exist(const string_type& x) {
			functor f(x);
			for (size_type i = 0; i < f.size(); ++i) {
				if (v_.find(f.at(i)) != v_.end()) return true;
			}
			return false;
		}
		
		bool exist(const char_type* x) {
			string_type tmp(x);
			return this->exist(tmp);
		}
		
		bool exist(char_type x) {
			string_type tmp(1, x);
			return this->exist(tmp);
		}
		
#if !defined(__BORLANDC__)
		template <class ValueT>
		basic_argument& operator()(ValueT& dest, size_type n = 0) {
			return this->get_param(dest, n);
		}
#endif
		
		template <class ValueT>
		basic_argument& operator()(const string_type& key, ValueT& dest, size_type n = 0) {
			return this->get_param(key, dest, n);
		}
		
		template <class ValueT>
		basic_argument& operator()(const char_type* key, ValueT& dest, size_type n = 0) {
			string_type tmp(key);
			return (*this)(tmp, dest, n);
		}
		
		template <class ValueT>
		basic_argument& operator()(const char_type& key, ValueT& dest, size_type n = 0) {
			string_type tmp(1, key);
			return (*this)(tmp, dest, n);
		}
		
	private:
		typedef basic_tokenizer<escape_separator<CharT, Traits>, string_type> functor;
		
		char_type identifier_;
		string_type program_;
		container head_;
		map_type v_;
		
		bool set_param(const key_type& x, const string_type& s) {
			typedef typename map_type::value_type value_type;
			typedef typename map_type::iterator iterator;
			
			iterator pos = v_.find(x);
			if (pos == v_.end()) {
				container v;
				if (!s.empty()) v.push_back(s);
				std::pair<iterator, bool> r = v_.insert(value_type(x, v));
				return r.second;
			}
			else if (!s.empty()) pos->second.push_back(s);
			return true;
		}
		
		template <class ValueT>
		basic_argument& get_param(ValueT& dest, size_type n) {
			if (n < this->head().size()) dest = lexical_cast<ValueT>(this->head().at(n));
			return *this;
		}
		
		template <class ValueT>
		basic_argument& get_param(const string_type& key, ValueT& dest, size_type n) {
			functor f(key);
			for (size_type i = 0; i < f.size(); ++i) {
				if (this->exist(f.at(i)) && n < v_[f.at(i)].size()) {
					dest = lexical_cast<ValueT>(v_[f.at(i)][n]);
				}
			}
			return *this;
		}
	};
	
	typedef basic_argument<char> argument;
#ifdef CLX_USE_WCHAR
	typedef basic_argument<wchar_t> wargument;
#endif // CLX_USE_WCHAR
}

#endif // CLX_ARGUMENT_H
