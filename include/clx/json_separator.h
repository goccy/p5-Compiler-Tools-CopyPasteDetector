/* ------------------------------------------------------------------------- */
/*
 *  json_separator.h
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
 *  Last-modified: Sat 19 Jun 2010 01:58:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_JSON_SEPARATOR_H
#define CLX_JSON_SEPARATOR_H

#include "config.h"
#include <cctype>
#include <iterator>
#include <stdexcept>
#include "predicate.h"
#include "lexical_cast.h"
#include "utf8.h"

namespace clx {
	/* --------------------------------------------------------------------- */
	//  json_control_charset
	/* --------------------------------------------------------------------- */
	template <class CharT>
	struct json_control_charset {
		typedef CharT char_type;
		
		char_type escape;
		char_type quot;
		char_type comma;
		char_type colon;
		char_type lbrace;
		char_type rbrace;
		char_type lbracket;
		char_type rbracket;
		
		char_type backspace;
		char_type ff;
		char_type lf;
		char_type cr;
		char_type tab;
		char_type unicode;
		
		json_control_charset() :
			escape(LITERAL('\\')), quot(LITERAL('"')), comma(LITERAL(',')), colon(LITERAL(':')),
			lbrace(LITERAL('{')), rbrace(LITERAL('}')),
			lbracket(LITERAL('[')), rbracket(LITERAL(']')),
			backspace(LITERAL('b')), ff(LITERAL('f')),lf(LITERAL('n')), cr(LITERAL('r')),
			tab(LITERAL('t')), unicode(LITERAL('u')) {}
	};
	
	namespace detail {
		/* ----------------------------------------------------------------- */
		/*
		 *  unicode
		 *
		 *  Current encoding is as follows:
		 *    - char -> UTF-8
		 *    - wchar_t -> UTF-16
		 */
		/* ----------------------------------------------------------------- */
		template <typename CharT>
		struct unicode {
			static std::basic_string<CharT> convert(int code) {
				return std::basic_string<CharT>(1, static_cast<CharT>(code));
			}
		};
		
		template <>
		struct unicode<char> {
			static std::basic_string<char> convert(int code) {
				std::basic_string<char> dest;
				std::insert_iterator<std::basic_string<char> > out(dest, dest.end());
				clx::utf8::encode(static_cast<char32_t>(code), out);
				return dest;
			}
		};
		
		/* ----------------------------------------------------------------- */
		//  next_string
		/* ----------------------------------------------------------------- */
		template <class InIter, class CharT>
		bool next_string(InIter& next, InIter last, std::basic_string<CharT>& dest) {
			json_control_charset<CharT> ctrl;
			
			++next;
			while (next != last && *next != ctrl.quot) {
				if (*next == ctrl.escape) {
					++next;
					if (*next == ctrl.unicode) {
						++next;
						std::basic_string<CharT> code;
						for (int i = 0; i < 3; ++i, ++next) code += *next;
						code += *next;
						dest += detail::unicode<CharT>::convert(lexical_cast<int>(code, std::ios::hex));
					}
					else if (*next == ctrl.backspace) dest += LITERAL('\b');
					else if (*next == ctrl.cr) dest += LITERAL('\r');
					else if (*next == ctrl.lf) dest += LITERAL('\n');
					else if (*next == ctrl.ff) dest += LITERAL('\f');
					else if (*next == ctrl.tab) dest += LITERAL('\t');
					else dest += *next;
				}
				else dest += *next;
				++next;
			}
			
			return next != last;
		}
		
		/* ----------------------------------------------------------------- */
		//  next_member
		/* ----------------------------------------------------------------- */
		template <class InIter, class CharT>
		bool next_member(InIter& next, InIter last,
			std::basic_string<CharT>& dest, CharT beg_char, CharT end_char) {
			int n = 1;
			while (next != last && n > 0) {
				dest += *next;
				++next;
				if (*next == beg_char) ++n;
				else if (*next == end_char) --n;
			}
			
			if (next != last) dest += *next;
			return next != last;
		}
	};
	
	/* --------------------------------------------------------------------- */
	//  json_separator
	/* --------------------------------------------------------------------- */
	template <
		class CharT,
		class Traits = std::char_traits<CharT>
	>
	class json_separator {
	public:
		typedef CharT char_type;
		typedef std::basic_string<CharT, Traits> string_type;
		
		void reset() {}
		
		/* ----------------------------------------------------------------- */
		/*
		 *  operator()
		 *
		 *  The JSON object allows only pair element. Rule of pair element
		 *  is "key: value", and key element allows only string (start
		 *  and end with '"' character). value element allows many kinds
		 *  of elements... (skip listing :p)
		 */
		/* ----------------------------------------------------------------- */
		template <class InIter, class Key, class Token>
		bool operator()(InIter& next, InIter last, Key& key, Token& dest) {
			static const clx::classified_functor space = is_space();
			
			if (next == last || (*next != ctrl.lbrace && *next != ctrl.quot)) return false;
			else if (*next == ctrl.lbrace) ++next;
			
			while (next != last && space(*next)) ++next;
			if (next == last || *next != ctrl.quot) return false;
			if (!detail::next_string(next, last, key)) return false;
			++next;
			
			if (next == last || *next != ctrl.colon) return false;
			++next;
			
			string_type tmp;
			while (next != last) {
				if (*next == ctrl.comma || *next == ctrl.rbrace) {
					++next;
					break;
				}
				else if (*next == ctrl.lbracket) {
					if (!detail::next_member(next, last, tmp, ctrl.lbracket, ctrl.rbracket)) return false;
				}
				else if (*next == ctrl.lbrace) {
					if (!detail::next_member(next, last, tmp, ctrl.lbrace, ctrl.rbrace)) return false;
				}
				else if (*next == ctrl.quot) {
					if (!detail::next_string(next, last, tmp)) return false;
				}
				else if (!space(*next)) tmp += *next;
				++next;
			}
			
			if (!tmp.empty()) dest = lexical_cast<Token>(tmp);
			return true;
		}
		
	private:
		json_control_charset<CharT> ctrl;
	};
}

#endif // CLX_JSON_SEPARATOR_H
