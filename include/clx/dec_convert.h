/* ------------------------------------------------------------------------- */
/*
 *  dec_convert.h
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
 *  Last-modified: Fri 12 Feb 2010 14:53:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_DEC_CONVERT_H
#define CLX_DEC_CONVERT_H

#include "config.h"
#include <iomanip>
#include <iterator>
#include <string>
#include <sstream>
#include "literal.h"
#include "mpl/bitmask.h"

namespace clx {
	/* --------------------------------------------------------------------- */
	//  basic_dec_encoder
	/* --------------------------------------------------------------------- */
	template <
		class CharT,
		class Traits = std::char_traits<CharT>
	>
	class basic_dec_encoder {
	public:
		typedef std::size_t size_type;
		typedef CharT char_type;
		typedef Traits traits;
		typedef std::basic_string<CharT, Traits> string_type;
		
		basic_dec_encoder() :
			prefix_(), suffix_() {}
		
		explicit basic_dec_encoder(const string_type& prefix) :
			prefix_(prefix), suffix_() {}
		
		explicit basic_dec_encoder(const char_type* prefix) :
			prefix_(prefix), suffix_() {}
		
		basic_dec_encoder(const string_type& prefix, const string_type& suffix) :
			prefix_(prefix), suffix_(suffix) {}
		
		basic_dec_encoder(const char_type* prefix, const char_type* suffix) :
			prefix_(prefix), suffix_(suffix) {}
		
		~basic_dec_encoder() throw() {}
		
		template <class OutIter>
		OutIter operator()(char_type c, OutIter out) {
			static const size_type width = (sizeof(char_type) > 1) ? 4 : 2;
			
			std::basic_stringstream<char_type, traits> ss;
			if (!prefix_.empty()) ss << prefix_;
			ss << std::setw(width) << std::setfill(LITERAL('0'));
			ss << (static_cast<size_type>(c) & mpl::lower_mask<sizeof(char_type) * 8>::value);
			if (!suffix_.empty()) ss << suffix_;
			
			std::istreambuf_iterator<char_type> first(ss);
			std::istreambuf_iterator<char_type> last;
			return std::copy(first, last, out);
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
		string_type prefix_;
		string_type suffix_;
	};
	
	typedef basic_dec_encoder<char, std::char_traits<char> > dec_encoder;
#ifdef CLX_USE_WCHAR
	typedef basic_dec_encoder<wchar_t, std::char_traits<wchar_t> > wdec_encoder;
#endif // CLX_USE_WCHAR
} // namespace clx

#endif // CLX_DEC_CONVERT_H
