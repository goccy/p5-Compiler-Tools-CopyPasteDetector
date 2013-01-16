/* ------------------------------------------------------------------------- */
/*
 *  scanner.h
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
 *  Last-modified: Tue 16 Jan 2007 04:56:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_SCANNER_H
#define CLX_SCANNER_H

#include "config.h"
#include <istream>
#include <string>
#include "tokenizer.h"
#include "lexical_cast.h"

namespace clx {
	/* --------------------------------------------------------------------- */
	//  basic_scanner
	/* --------------------------------------------------------------------- */
	template <
		class CharT,
		class Traits = std::char_traits<CharT>
	>
	class basic_scanner {
	public:
		typedef CharT char_type;
		typedef unsigned int size_type;
		typedef typename std::basic_string<CharT, Traits> string_type;
		
		template <class Ch, class Tr>
		explicit basic_scanner(std::basic_istream<Ch, Tr>& sin, const char_type* fmt) :
			f_(separator(fmt)) { this->scan(sin); }
		
		template <class Ch, class Tr>
		explicit basic_scanner(std::basic_istream<Ch, Tr>& sin, const string_type& fmt) :
			f_(separator(fmt)) { this->scan(sin); }
		
		explicit basic_scanner(const char_type* src, const char_type* fmt) :
			f_(separator(fmt)) { this->scan(src); }
		
		explicit basic_scanner(const string_type& src, const string_type& fmt) :
			f_(separator(fmt)) { this->scan(src); }
		
		virtual ~basic_scanner() throw() {}
		
		template <class ValueT>
		basic_scanner& operator%(ValueT& dest) {
			return this->assign(dest);
		}
		
		template <class ValueT>
		basic_scanner& operator()(size_type pos, ValueT& dest) {
			return this->assign(pos, dest);
		}
		
		template <class ValueT, class Assign>
		basic_scanner& operator()(size_type pos, ValueT& dest, Assign f) {
			return this->assign(pos, dest, f);
		}
		
	private:
		typedef format_separator<CharT, Traits> separator;
		typedef basic_tokenizer<separator, string_type> functor;
		typedef typename functor::const_iterator const_iterator;
		
		functor f_;
		const_iterator cur_;
		
		void scan(const string_type& src) {
			f_.assign(src);
			cur_ = f_.begin();
		}
		
		void scan(const char_type* src) {
			string_type tmp(src);
			this->scan(tmp);
		}
		
		template <class Ch, class Tr>
		void scan(std::basic_istream<Ch, Tr>& sin) {
			std::string tmp;
			std::getline(sin, tmp);
			this->scan(tmp);
		}
		
		template <class ValueT>
		basic_scanner& assign(ValueT& dest) {
			if (cur_ != f_.end()) {
				dest = lexical_cast<ValueT>(*cur_);
				++cur_;
			}
			return *this;
		}
		
		template <class ValueT>
		basic_scanner& assign(size_type pos, ValueT& dest) {
			if (pos < f_.size()) dest = lexical_cast<ValueT>(f_.at(pos));
			return *this;
		}
		
		template <class ValueT, class Assign>
		basic_scanner& assign(size_type pos, ValueT& dest, Assign f) {
			if (pos < f_.size()) f(f_.at(pos), dest);
			return *this;
		}
	};
	
	typedef basic_scanner<char> scanner;
#ifdef CLX_USE_WCHAR
	typedef basic_scanner<wchar_t> wscanner;
#endif // CLX_USE_WCHAR
}

#endif // CLX_SCANNER_H
