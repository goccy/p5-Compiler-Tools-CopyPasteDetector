/* ------------------------------------------------------------------------- */
/*
 *  sockstream.h
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
 *  Last-modified: Wed 03 Sep 2008 20:24:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_SOCKSTREAM_H
#define CLX_SOCKSTREAM_H

#include "config.h"
#include <iostream>
#include <string>
#include "sockbuf.h"

namespace clx {
	/* --------------------------------------------------------------------- */
	//  basic_sockstream
	/* --------------------------------------------------------------------- */
	template <
		class Socket,
		class CharT = char,
		class Traits = std::char_traits<CharT>
	>
	class basic_sockstream : public Socket, public std::basic_iostream<CharT, Traits> {
	private:
		typedef std::basic_iostream<CharT, Traits> iostream_type;
	public:
		typedef Socket socket_type;
		typedef CharT char_type;
		typedef std::basic_string<CharT, Traits> string_type;
		typedef basic_sockbuf<Socket, CharT, Traits> streambuf_type;
		typedef typename streambuf_type::size_type size_type;
		
		enum { nbuf = 65536 };
		
		// constructor and destructor
		explicit basic_sockstream(const socket_type& s, size_type n = nbuf) :
			socket_type(s), iostream_type(0), sbuf_(n) {
			this->reset();
		}
		
		explicit basic_sockstream(const char_type* host, int port, size_type n = nbuf) :
			socket_type(host, port), iostream_type(0), sbuf_(n) {
			this->reset();
		}
		
		explicit basic_sockstream(const string_type& host, int port, size_type n = nbuf) :
			socket_type(host, port), iostream_type(0), sbuf_(n) {
			this->reset();
		}
		
		virtual ~basic_sockstream() throw() { this->rdbuf(0); }
		
	private:
		streambuf_type sbuf_;
		
		void reset() {
			sbuf_.socket(*this);
			this->rdbuf(&sbuf_);
			if (!this->is_open()) this->setstate(std::ios::badbit);
		}
	};
}

#endif // CLX_SOCKSTREAM_H
