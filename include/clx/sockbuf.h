/* ------------------------------------------------------------------------- */
/*
 *  sockbuf.h
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
 *  Last-modified: Wed 13 Jan 2010 05:36:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_SOCKBUF_H
#define CLX_SOCKBUF_H

#include "config.h"
#include <streambuf>
#include <vector>
#include <cstring>

namespace clx {
	/* --------------------------------------------------------------------- */
	//  basic_sockbuf
	/* --------------------------------------------------------------------- */
	template <
		class Socket,
		class CharT = char,
		class Traits = std::char_traits<CharT>
	>
	class basic_sockbuf : public std::basic_streambuf<CharT, Traits> {
	private:
		typedef std::basic_streambuf<CharT, Traits> super;
	public:
		typedef Socket socket_type;
		typedef CharT char_type;
		typedef Traits traits;
		typedef typename std::vector<CharT> container;
		typedef typename container::size_type size_type;
		typedef typename Traits::int_type int_type;
		
		enum { npback = 8, nbuf = 65536 };
		
		// constructor and destructor
		explicit basic_sockbuf(size_type n) :
			super(), sock_(), buffer_(n) {
			this->setg(&buffer_.at(0), &buffer_.at(npback), &buffer_.at(npback));
		}
		
		explicit basic_sockbuf(socket_type& s, size_type n = nbuf) :
			super(), sock_(&s), buffer_(n) {
			this->setg(&buffer_.at(0), &buffer_.at(npback), &buffer_.at(npback));
		}
		
		virtual ~basic_sockbuf() throw() {}
		
		void socket(socket_type& s) { sock_ = &s; }
		
	protected:
		virtual int_type overflow(int_type c) {
			if (!traits::eq_int_type(c, traits::eof())) {
				char tmp = static_cast<char>(c);
				if (this->xsputn((char_type*)&tmp, 1) == 0) return traits::eof();
			}
			return traits::not_eof(c);
		}
		
		virtual std::streamsize xsputn(const char_type* s, std::streamsize n) {
			std::streamsize transferred = 0;
			while (transferred < n) {
				int byte = static_cast<int>(n - transferred) * sizeof(char_type);
				int l = sock_->send((char*)s, byte);
				if (l <= 0) break;
				transferred += (l / sizeof(char_type));
				s += (l / sizeof(char_type));
			}
			return transferred;
		}
		
		virtual int_type underflow() {
			if (this->gptr() == this->egptr()) {
				// copy the previous read string to the putback field
				int n = static_cast<int>(this->gptr() - this->eback());
				if (n > npback) n = npback;
				std::memcpy(&buffer_.at(npback - n), this->gptr() - n, n);
				
				int byte = static_cast<int>((buffer_.size() - npback) * sizeof(char_type));
				int l = sock_->recv(reinterpret_cast<char*>(&buffer_.at(npback)), byte);
				if (l <= 0) return traits::eof();
				this->setg(&buffer_.at(npback - n), &buffer_.at(npback), &buffer_.at(npback + l));
				return traits::to_int_type(*this->gptr());
			}
			else return traits::eof();
		}
		
		virtual int_type uflow() {
			int_type c = this->underflow();
			if (!traits::eq_int_type(c, traits::eof())) this->gbump(1);
			return c;
		}
		
		virtual int sync() {
			return this->overflow(traits::eof());
		}
		
	private:
		socket_type* sock_;
		container buffer_;
	};
};

#endif // CLX_SOCKBUF_H
