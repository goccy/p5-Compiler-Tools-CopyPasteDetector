/* ------------------------------------------------------------------------- */
/*
 *  zstream.h
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
 *  Copyright (C) 1995-2005 Jean-loup Gailly and Mark Adler.
 *
 *  The source code is derived from zlib.
 *  (See the following URL: http://zlib.net/zlib_license.html)
 *
 *  Last-modified: Tue 02 Jun 2009 23:19:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_ZSTREAM_H
#define CLX_ZSTREAM_H

#include "config.h"
#include <istream>
#include <ostream>
#include <string>
#include "zbuf.h"

namespace clx {
	/* --------------------------------------------------------------------- */
	//  basic_unzstream
	/* --------------------------------------------------------------------- */
	template <
		class CharT = char,
		class Traits = std::char_traits<CharT>
	>
	class basic_unzstream : public std::basic_istream<CharT, Traits> {
	public:
		typedef std::basic_istream<CharT, Traits> stream_type;
		typedef basic_unzstreambuf<CharT, Traits> streambuf_type;
		typedef typename streambuf_type::size_type size_type;
		
		basic_unzstream(stream_type& in, size_type n = 4096) :
			stream_type(0), zbuf_(in, n) {
			this->rdbuf(&zbuf_);
		}
		
		virtual ~basic_unzstream() throw() { this->rdbuf(0); }
		
	private:
		streambuf_type zbuf_;
	};
	
	/* --------------------------------------------------------------------- */
	//  basic_zstream
	/* --------------------------------------------------------------------- */
	template <
		int CompressRate,
		class CharT = char,
		class Traits = std::char_traits<CharT>
	>
	class basic_zstream : public std::basic_ostream<CharT, Traits> {
	public:
		typedef std::basic_ostream<CharT, Traits> stream_type;
		typedef basic_zstreambuf<CompressRate, CharT, Traits> streambuf_type;
		typedef typename streambuf_type::size_type size_type;
		
		basic_zstream(stream_type& out, size_type n = 4096) :
			stream_type(0), zbuf_(out, n) {
			this->rdbuf(&zbuf_);
		}
		
		virtual ~basic_zstream() throw() {
			this->finish();
			this->rdbuf(0);
		}
		
		void finish() { zbuf_.finish(); }
		
	private:
		streambuf_type zbuf_;
	};
	
	typedef basic_unzstream<char> unzstream;
	typedef basic_zstream<Z_DEFAULT_COMPRESSION, char> zstream;
}

#endif // CLX_ZSTREAM_H
