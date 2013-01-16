/* ------------------------------------------------------------------------- */
/*
 *  literal.h
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
 *  Distributed under the license of TrickLibrary, (See the following URL:
 *  http://tricklib.com/license.htm)
 *
 *  Last-modified: Tue 26 Aug 2008 03:26:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_LITERAL_H
#define CLX_LITERAL_H

#include "config.h"

#ifdef CLX_USE_WCHAR
namespace clx {
	/* --------------------------------------------------------------------- */
	//  basic_literals
	/* --------------------------------------------------------------------- */
	template <class CharT, class WideT>
	class basic_literals {
	public:
		typedef CharT char_type;
		typedef WideT wide_type;
		
		basic_literals(CharT c, WideT wc) :
			c_(c), wc_(wc) {}
		
		operator char_type() const { return c_; }
		operator wide_type() const { return wc_; }
		
	private:
		char_type c_;
		wide_type wc_;
	};
	
	typedef basic_literals<const char, const wchar_t> char_literals;
	typedef basic_literals<const char*, const wchar_t*> string_literals;
	
	/* --------------------------------------------------------------------- */
	//  generate_char_literals
	/* --------------------------------------------------------------------- */
	inline char_literals generate_literals(const char c, const wchar_t wc) {
		return char_literals(c, wc);
	}
	
	inline string_literals generate_literals(const char* c, const wchar_t* wc) {
		return string_literals(c, wc);
	}
}
#define LITERAL(X) clx::generate_literals(X, L##X)
#else
#define LITERAL(X) X
#endif // CLX_USE_WCHAR

#endif // CLX_LITERAL_H
