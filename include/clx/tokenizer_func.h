/* ------------------------------------------------------------------------- */
/*
 *  tokenizer_func.h
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
 *  Last-modified: Wed 02 Sep 2009 02:02:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_TOKENIZER_FUNC_H
#define CLX_TOKENIZER_FUNC_H

#include "config.h"
#include <algorithm>
#include <cstdlib>
#include <cctype>
#include <string>
#include "literal.h"
#include "lexical_cast.h"

namespace clx {
	/* --------------------------------------------------------------------- */
	/*!
	 *  char_separator
	 *
	 *  The char_separator class breaks a sequence of characters into
	 *  tokens based on the character delimiters.
	 */
	/* --------------------------------------------------------------------- */
	template <
		class CharT,
		class Traits = std::char_traits<CharT>
	>
	class char_separator {
	public:
		typedef CharT char_type;
		typedef typename std::basic_string<CharT, Traits> string_type;
		
		// constructor and destructor
		char_separator() : c_(1, LITERAL(' ')), emtok_(false) {}
		
		explicit char_separator(char_type c, bool x = false) :
			c_(1, c), emtok_(false) {}
		
		explicit char_separator(const char_type* c, bool x = false) :
			c_(c), emtok_(x) {}
		
		explicit char_separator(const string_type& c, bool x = false) :
			c_(c), emtok_(x) {}
		
		// operator
		void reset() { return; }
		
		bool empty_token(bool x) {
			emtok_ = x;
			return emtok_;
		}
		
		template <class InIter, class Token>
		bool operator()(InIter& next, InIter last, Token& dest) {
			if (next == last) return false;
			if (!emtok_) { // invalid empty token, so skip continuous delimiters.
				while (std::find(c_.begin(), c_.end(), *next) != c_.end()) {
					++next;
					if (next == last) return false;
				}
			}
			
			string_type tmp;
			while (next != last) { // obtain the next token.
				if (std::find(c_.begin(), c_.end(), *next) != c_.end()) {
					++next;
					break;
				}
				tmp += *next;
				++next;
			}
			if (!tmp.empty()) dest = lexical_cast<Token>(tmp);
			
			return true;
		}
		
	private:
		string_type c_;
		bool emtok_;
	
	};
	
	/* --------------------------------------------------------------------- */
	/*!
	 *  escape_separator
	 *
	 *  The escape_separator class is a super-set of what is commonly known
	 *  as a comma separated value (csv) list.
	 */
	/* --------------------------------------------------------------------- */
	template <
		class CharT,
		class Traits = std::char_traits<CharT>
	>
	class escape_separator {
	public:
		typedef CharT char_type;
		typedef typename std::basic_string<CharT, Traits> string_type;
		
		// constructor and destructor
		escape_separator() : c_(1, LITERAL(',')), quote_(1, LITERAL('\"')), esc_(1, LITERAL('\\')), emtok_(false) {}
		
		explicit escape_separator(char_type c, char_type q, char_type e, bool x = false) :
			c_(1, c), quote_(1, q), esc_(1, e), emtok_(x) {}
		
		explicit escape_separator(const char_type* c, const char_type* q, const char_type* e, bool x = false) :
			c_(c), quote_(q), esc_(e), emtok_(x) {}
		
		explicit escape_separator(const string_type& c, const string_type& q, const string_type& e, bool x = false) :
			c_(c), quote_(q), esc_(e), emtok_(x) {}
		
		// operator
		void reset() { return; }
		
		bool empty_token(bool x) {
			emtok_ = x;
			return emtok_;
		}
		
		template <class InIter, class Token>
		bool operator()(InIter& next, InIter last, Token& dest) {
			bool inq = false;
			
			if (next == last) return false;
			if (!emtok_) { // invalid empty token, so skip continuous delimiters.
				while (std::find(c_.begin(), c_.end(), *next) != c_.end()) {
					++next;
					if (next == last) return false;
				}
			}
			
			string_type tmp;
			while (next != last) { // obtain the next token.
				if (std::find(quote_.begin(), quote_.end(), *next) != quote_.end()) inq = !inq;
				else if (!inq && std::find(c_.begin(), c_.end(), *next) != c_.end()) {
					++next;
					break;
				}
				else {
					if (std::find(esc_.begin(), esc_.end(), *next) != esc_.end()) ++next;
					if (next == last) return false;
					tmp += *next;
				}
				++next;
			}
			if (!tmp.empty()) dest = lexical_cast<Token>(tmp);
			
			return true;
		}
		
	private:
		string_type c_;
		string_type quote_;
		string_type esc_;
		bool emtok_;
	};
	
	/* --------------------------------------------------------------------- */
	/*!
	 *  format_separator
	 *
	 *  The format separator class is split a string according to the
	 *  given format. The behavior of this class is similar to scanf()
	 *  function.
	 */
	/* --------------------------------------------------------------------- */
	template <
		class CharT,
		class Traits = std::char_traits<CharT>
	>
	class format_separator {
	public:
		typedef CharT char_type;
		typedef typename std::basic_string<CharT, Traits> string_type;
		typedef typename string_type::iterator iterator;
		
		// constructor and destructor
		format_separator() : fmt_(LITERAL("%s")), ids_(1, LITERAL('s')), skipws_(true) {
			this->init();
		}
		
		explicit format_separator(const char_type* fmt, char_type idset = LITERAL('s'), bool x = true) :
			fmt_(fmt), ids_(1, idset), skipws_(x) {
			this->init();
			cur_ = fmt_.begin();
		}
		
		explicit format_separator(const string_type& fmt, char_type idset = LITERAL('s'), bool x = true) :
			fmt_(fmt), ids_(1, idset), skipws_(x) {
			this->init();
			cur_ = fmt_.begin();
		}
		
		explicit format_separator(const char_type* fmt, const char_type* idset, bool x = true) :
			fmt_(fmt), ids_(idset), skipws_(x) {
			this->init();
			cur_ = fmt_.begin();
		}
		
		explicit format_separator(const string_type& fmt, const string_type& idset, bool x = true) :
			fmt_(fmt), ids_(idset), skipws_(x) {
			this->init();
			cur_ = fmt_.begin();
		}
		
		virtual ~format_separator() {}
		
		// operator
		void reset() { cur_ = fmt_.begin(); }
		
		bool skipws(bool x) {
			skipws_ = x;
			return skipws_;
		}
		
		template <class InIter, class Token>
		bool operator()(InIter& next, InIter last, Token& dest) {
			char_type c;
			return this->next_token(next, last, c, dest);
		}
		
		// method for basic_tokenmap<>
		template <class InIter, class Key, class Token>
		bool operator()(InIter& next, InIter last, Key& key, Token& dest) {
			return this->next_token(next, last, key, dest);
		}
		
	private:
		char_type rep; // '%'
		char_type ws;  // ' '
		
		string_type fmt_;
		string_type ids_;
		bool skipws_;
		iterator cur_;
		
		void init() {
			rep = LITERAL('%');
			ws  = LITERAL(' ');
		}
		
		template <class InIter, class Key, class Token>
		bool next_token(InIter& next, InIter last, Key& key, Token& dest) {
			while (next != last && cur_ != fmt_.end()) {
				if (*cur_ == rep && cur_ + 1 != fmt_.end() && *(cur_ + 1) != rep) {
					string_type flag, normal;
					if (!this->get_format(flag, key)) break;
					this->get_normal(normal);
					string_type tmp;
					tmp = this->get(next, last, normal, flag);
					if (!tmp.empty()) dest = lexical_cast<Token>(tmp);
					return true;
				}
				else {
					string_type normal;
					if (!this->get_normal(normal) || !this->compare(next, last, normal)) break;
				}
			}
			return false;
		}
		
		template <class InIter>
		string_type get(InIter& next, InIter last,
			const string_type& normal, const string_type& flag) {
			int n = -1;
			if (!flag.empty() && std::isdigit(flag.at(0))) n = lexical_cast<int>(flag);
			if (skipws_) {
				while (next != last && *next == ' ') ++next;
			}
			
			string_type dest;
			while (next != last && n != 0) {
				InIter prev = next;
				if (this->compare(next, last, normal)) break;
				next = prev;
				dest += *next;
				++next;
				--n;
			}
			
			// skip characters until the next "%"
			if (n == 0) {
				InIter tmp = next;
				if (!this->compare(next, last, normal)) next = tmp;
			}
			
			return dest;
		}
		
		// get next normal string
		bool get_normal(string_type& normal) {
			normal.clear();
			while (cur_ != fmt_.end()) {
				if (*cur_ == rep) {
					if (cur_ + 1 != fmt_.end() && *(cur_ + 1) == rep) ++cur_;
					else break;
				}
				normal += *cur_;
				++cur_;
			}
			
			if (normal.empty()) return false;
			return true;
		}
		
		// get formatted string (start with "%")
		template <class Key>
		bool get_format(string_type& flag, Key& key) {
			++cur_;
			flag.clear();
			while (cur_ != fmt_.end() && ids_.find(*cur_) == string_type::npos) {
				flag += *cur_;
				++cur_;
			}
			if (cur_ == fmt_.end()) return false;
			key = *cur_;
			++cur_;
			return true;
		}
		
		template <class InIter>
		bool compare(InIter& next, InIter last, const string_type& s) {
			if (s.empty()) return false;
			typedef typename string_type::const_iterator string_iterator;
			string_iterator spos = s.begin();
			
			while (next != last) {
				if (skipws_ && *next == ws && *spos != ws) ++next;
				else if (*next != *spos) return false;
				else {
					++next;
					++spos;
					if (spos == s.end()) break;
				}
			}
			
			return true;
		}
	};
}

#endif // CLX_TOKENIZER_FUNC_H
