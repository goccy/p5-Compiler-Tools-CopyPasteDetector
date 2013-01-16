/* ------------------------------------------------------------------------- */
/*
 *  uri_convert.h
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
 *  Last-modified: Mon 01 Mar 2010 19:44:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_URI_CONVERT_H
#define CLX_URI_CONVERT_H

#include "config.h"
#include <algorithm>
#include <iomanip>
#include <iterator>
#include <string>
#include <sstream>
#include <stdexcept>
#include "literal.h"
#include "lexical_cast.h"
#include "predicate.h"
#include "mpl/bitmask.h"

// temporarily disable warnings that expression is always true/false.
#if defined(__BORLANDC__)
#pragma warn -8008
#pragma warn -8066
#endif

namespace clx {
	/* --------------------------------------------------------------------- */
	//  basic_uri_encoder
	/* --------------------------------------------------------------------- */
	template <
		class CharT,
		class Traits = std::char_traits<CharT>
	>
	class basic_uri_encoder {
	public:
		typedef std::size_t size_type;
		typedef CharT char_type;
		typedef Traits traits;
		typedef std::basic_string<CharT, Traits> string_type;
		
		explicit basic_uri_encoder(bool lower = true) :
			f_(clx::is_any_of((const char_type*)LITERAL("!#$&'()*+,-./:;=?@_~"))),
			plus_(false), lower_(lower) {}
		
		explicit basic_uri_encoder(const string_type& symbols, bool space_to_plus, bool lower = true) :
			f_(clx::is_any_of(symbols)), plus_(space_to_plus), lower_(lower) {}
		
		explicit basic_uri_encoder(const char_type* symbols, bool space_to_plus, bool lower = true) :
			f_(clx::is_any_of(symbols)), plus_(space_to_plus), lower_(lower) {}
		
		~basic_uri_encoder() throw() {}
		
		template <class OutIter>
		OutIter operator()(char_type c, OutIter out) {
			static const clx::classified_functor alnum = clx::is_alnum();
			static const size_type width = (sizeof(char_type) > 1) ? 4 : 2;
			
			if (alnum(c) || f_(c)) *out++ = c;
			else if (c == 0x20 && plus_) *out++ = LITERAL('+');
			else {
				std::basic_stringstream<char_type, traits> ss;
				ss << LITERAL('%');
				if (sizeof(char_type) > 1) ss << LITERAL('u');
				if (!lower_) ss << std::uppercase;
				ss << std::setw(width) << std::setfill((char_type)LITERAL('0')) << std::hex;
				ss << (static_cast<size_type>(c) & mpl::lower_mask<sizeof(char_type) * 8>::value);
				
				std::istreambuf_iterator<char_type> first(ss);
				std::istreambuf_iterator<char_type> last;
				out = std::copy(first, last, out);
			}
			
			return out;
		}
		
		template <class OutIter>
		OutIter reset(OutIter out) {
			return out;
		}
		
		template <class OutIter>
		OutIter finish(OutIter out) {
			return out;
		}
		
	private:
		charset_functor<char_type> f_;
		bool plus_;
		bool lower_;
	};
	
	/* --------------------------------------------------------------------- */
	//  basic_uri_decoder
	/* --------------------------------------------------------------------- */
	template <
		class CharT,
		class Traits = std::char_traits<CharT>
	>
	class basic_uri_decoder {
	public:
		typedef std::size_t size_type;
		typedef CharT char_type;
		typedef Traits traits;
		typedef std::basic_string<CharT, Traits> string_type;
		
		basic_uri_decoder() :
			status_(0), buffer_() {}
		
		~basic_uri_decoder() throw() {}
		
		template <class OutIter>
		OutIter operator()(char_type c, OutIter out) {
			switch (status_) {
			case 1: // encoded ASCII character
				if (c == LITERAL('%')) throw std::runtime_error("invalid encoded URI");
				if (c == LITERAL('u')) status_ = 2;
				else {
					buffer_ += c;
					if (buffer_.size() >= 2) out = this->output(out);
				}
				break;
			case 2: // encoded Multi-byte character
				if (c == LITERAL('%')) throw std::runtime_error("invalid encoded URI");
				buffer_ += c;
				if (buffer_.size() >= 4) out = this->output(out);
				break;
			default:
				if (c == LITERAL('%')) status_ = 1;
				else if (c == LITERAL('+')) *out++ = 0x20;
				else *out++ = c;
				break;
			}
			
			return out;
		}
		
		template <class OutIter>
		OutIter reset(OutIter out) {
			status_ = 0;
			buffer_.clear();
			return out;
		}
		
		template <class OutIter>
		OutIter finish(OutIter out) {
			if (!buffer_.empty()) throw std::runtime_error("invalid encoded URI");
			return out;
		}
		
	private:
		size_type status_;
		string_type buffer_;
		
		template <class OutIter>
		OutIter output(OutIter out) {
			int value = lexical_cast<int>(buffer_, std::ios::hex);
			*out++ = static_cast<char_type>(value);
			buffer_.clear();
			status_ = 0;
			return out;
		}
	};
	
	typedef basic_uri_encoder<char, std::char_traits<char> > uri_encoder;
	typedef basic_uri_decoder<char, std::char_traits<char> > uri_decoder;
#ifdef CLX_USE_WCHAR
	typedef basic_uri_encoder<wchar_t, std::char_traits<wchar_t> > wuri_encoder;
	typedef basic_uri_decoder<wchar_t, std::char_traits<wchar_t> > wuri_decoder;
#endif // CLX_USE_WCHAR
} // namespace clx

#if defined(__BORLANDC__)
#pragma warn .8008
#pragma warn .8066
#endif

#endif // CLX_URI_CONVERT_H
