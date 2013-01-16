/* ------------------------------------------------------------------------- */
/*
 *  format.h
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
 *  Last-modified: Fri 30 Jul 2010 20:44:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_FORMAT_H
#define CLX_FORMAT_H

#include "config.h"
#include <cstdlib>
#include <cctype>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <ostream>
#include <string>
#include "literal.h"
#include "lexical_cast.h"

namespace clx {
	/* --------------------------------------------------------------------- */
	//  format_charset
	/* --------------------------------------------------------------------- */
	template <class CharT>
	struct format_charset {
		typedef CharT char_type;
		
		char_type rep;
		char_type lpos;
		char_type ws;
		char_type sign;
		char_type zero;
		char_type prefix;
		char_type dot;
		
		char_type oct;
		char_type hex;
		char_type exp;
		char_type fix;
		
		format_charset() :
			rep(LITERAL('%')), lpos(LITERAL('-')), ws(LITERAL(' ')),
			sign(LITERAL('+')), zero(LITERAL('0')), prefix(LITERAL('#')), dot(LITERAL('.')),
			oct(LITERAL('o')), hex(LITERAL('x')), exp(LITERAL('e')), fix(LITERAL('f')) {}
	};
	
	/* --------------------------------------------------------------------- */
	//  basic_format
	/* --------------------------------------------------------------------- */
	template <
		class CharT,
		class Traits = std::char_traits<CharT>
	>
	class basic_format {
	public:
		typedef CharT char_type;
		typedef unsigned int size_type;
		typedef typename std::basic_string<CharT, Traits> string_type;
		
		// constructor and destructor
		basic_format() : str_(), fmt_() {}
		
		explicit basic_format(const string_type& fmt) :
			str_(), fmt_(fmt) {
			this->reset();
		}
		
		explicit basic_format(const char_type* fmt) :
			str_(), fmt_(fmt) {
			this->reset();
		}
		
		basic_format& operator()(const string_type& fmt) {
			fmt_ = fmt;
			this->reset();
			return *this;
		}
		
		basic_format& operator()(const char_type* fmt) {
			string_type tmp(fmt);
			return (*this)(tmp);
		}
		
		template <class ValueT>
		basic_format& operator%(const ValueT& x) {
			if (*pos_ == ctrl.rep && pos_ + 1 != fmt_.end() && *(pos_ + 1) != ctrl.rep) {
				string_type flag;
				while (++pos_ != fmt_.end() && types_.find(*pos_) == string_type::npos) flag += *pos_;
				if (pos_ == fmt_.end()) return *this; // uncorrect format
				str_ += this->put(x, *pos_++, flag);
			}
			
			while (pos_ != fmt_.end()) {
				if (*pos_ == ctrl.rep) {
					if (pos_ + 1 != fmt_.end() && *(pos_ + 1) == ctrl.rep) ++pos_;
					else break;
				}
				str_ += *pos_;
				++pos_;
			}
			
			return *this;
		}
		
		string_type str() const { return str_; }
		
		friend std::basic_ostream<char_type>& operator<<(
			std::basic_ostream<char_type>& sout, const basic_format<char_type>& f) {
			sout << f.str_;
			return sout;
		}
		
	private:
		typedef typename string_type::const_iterator const_iterator;
		typedef typename std::basic_stringstream<CharT, Traits> internal_stream;
		
		format_charset<char_type> ctrl;
		
		string_type types_;
		string_type str_;
		string_type fmt_;
		const_iterator pos_;
		
		void reset() {
			str_.clear();
			pos_ = fmt_.begin();
			types_ = LITERAL("diouxXeEfFgGcs");
			
			while (pos_ != fmt_.end()) {
				if (*pos_ == ctrl.rep) {
					if (pos_ + 1 != fmt_.end() && *(pos_ + 1) == ctrl.rep) ++pos_;
					else break;
				}
				str_ += *pos_;
				++pos_;
			}
		}
		
		template <class ValueT>
		string_type put(const ValueT& x, char_type c, const string_type& flag) {
			internal_stream ss;
			
			this->setopt(ss, flag);
			this->settype(ss, c);
			ss << x;
			
			return ss.str();
		}
		
		void setopt(internal_stream& ss, const string_type& flag) {
			bool lp = false;
			for (const_iterator pos = flag.begin(); pos != flag.end(); ++pos) {
				char_type c = *pos;
				if (c == ctrl.lpos) {
					lp = true;
					ss << std::setfill(ctrl.ws);
					ss << std::left;
				}
				else if (c == ctrl.prefix) ss << std::showbase;
				else if (c == ctrl.sign) ss << std::showpos;
				else if (c == ctrl.ws) ss << std::setfill(c);
				else if (c == ctrl.zero && !lp) ss << std::setfill(c);
				else if (c == ctrl.dot) {
					this->setprecision(ss, ++pos, flag.end());
					if (pos == flag.end()) return;
				}
				else {
					if (std::isdigit(c)) this->setwidth(ss, pos, flag.end());
					if (pos == flag.end()) return;
				}
			}
		}
		
		void setwidth(internal_stream& ss, const_iterator& cur, const_iterator last) {
			std::string digit;
			for (; cur != last && std::isdigit(*cur); ++cur) digit += *cur;
			if (!digit.empty()) ss << std::setw(lexical_cast<int>(digit));
			if (cur != last && *cur == ctrl.dot) this->setprecision(ss, ++cur, last);
		}
		
		void setprecision(internal_stream& ss, const_iterator& cur, const_iterator last) {
			std::string digit;
			for (; cur != last && std::isdigit(*cur); ++cur) digit += *cur;
			if (!digit.empty()) ss << std::setprecision(lexical_cast<int>(digit));
		}
		
		void settype(internal_stream& ss, char_type c) {
			char_type ctype;
			if (std::isupper(c)) ss << std::uppercase;
			ctype = std::tolower(c);
			
			if (ctype == ctrl.oct) ss << std::oct;
			else if (ctype == ctrl.hex) ss << std::hex;
			else if (ctype == ctrl.exp) ss << std::scientific;
			else if (ctype == ctrl.fix) ss << std::setiosflags(std::ios::fixed);
		}
	};
	
	/* --------------------------------------------------------------------- */
	//  str
	/* --------------------------------------------------------------------- */
	template <class Ch, class Tr>
	std::basic_string<Ch, Tr> str(const basic_format<Ch, Tr>& f) {
		return f.str();
	}
	
	typedef basic_format<char> format;
#ifdef CLX_USE_WCHAR
	typedef basic_format<wchar_t> wformat;
#endif // CLX_USE_WCHAR
}
#endif // CLX_FORMAT_H
