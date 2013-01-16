/* ------------------------------------------------------------------------- */
/*
 *  zbuf.h
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
 *  Copyright (C) 1995-2005 Jean-loup Gailly and Mark Adler.
 *
 *  The source code is derived from zlib.
 *  (See the following URL: http://zlib.net/zlib_license.html)
 *
 *  Last-modified: Fri 30 Jul 2010 14:45:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_ZBUF_H
#define CLX_ZBUF_H

#include "config.h"
#include <istream>
#include <ostream>
#include <string>
#include <vector>
#include <zlib.h>

#ifdef CLX_ENABLE_AUTOLINK
#if defined(_MSC_VER) && (_MSC_VER >= 1200) || defined(__BORLANDC__)
#pragma comment(lib, "zlib.lib")
#endif
#endif // CLX_ENABLE_AUTOLINK

namespace clx {
	/* --------------------------------------------------------------------- */
	/*
	 *  basic_zstreambuf
	 *
	 *  CompressRate can be set to the value from zero to nine.
	 *  Note that Z_DEFAULT_COMPRESSION means six.
	 */
	/* --------------------------------------------------------------------- */
	template <
		int CompressRate,
		class CharT = char,
		class Traits = std::char_traits<CharT>
	>
	class basic_zstreambuf : public std::basic_streambuf<CharT, Traits> {
	public:
		typedef unsigned char byte_type;
		typedef CharT char_type;
		typedef Traits traits;
		typedef std::basic_ostream<CharT, Traits> stream_type;
		typedef std::vector<CharT> container;
		typedef typename container::size_type size_type;
		typedef typename Traits::int_type int_type;
		
		basic_zstreambuf(stream_type& out, size_type n) :
			out_(out), buffer_(n, 0), z_(), flush_(Z_NO_FLUSH) {
			z_.zalloc = Z_NULL;
			z_.zfree = Z_NULL;
			z_.opaque = Z_NULL;
			deflateInit(&z_, CompressRate);
			z_.next_out = reinterpret_cast<byte_type*>(&buffer_[0]);
			z_.avail_out = buffer_.size() * sizeof(char_type);
		}
		
		virtual ~basic_zstreambuf() throw() {
			this->finish();
			deflateEnd(&z_);
		}
		
		void finish() {
			if (flush_ != Z_FINISH) {
				flush_ = Z_FINISH;
				this->xsputn(NULL, 0);
			}
		}
		
	protected:
		virtual int_type overflow(int_type c) {
			if (!traits::eq_int_type(c, traits::eof())) {
				char_type tmp = static_cast<char_type>(c);
				if (this->xsputn(&tmp, 1) == 0) return traits::eof();
			}
			return traits::not_eof(c);
		}
		
		virtual std::streamsize xsputn(const char_type* s, std::streamsize n) {
			z_.next_in = reinterpret_cast<byte_type*>(const_cast<char_type*>(s));
			z_.avail_in = static_cast<uInt>(n);
			
			while (flush_ == Z_FINISH || z_.avail_in > 0) {
				int status = deflate(&z_, flush_);
				if (status == Z_STREAM_END) {
					out_.write(&buffer_[0], buffer_.size() - z_.avail_out / sizeof(char_type));
					break;
				}
				else if (status != Z_OK) return 0;
				
				if (z_.avail_out == 0) {
					out_.write(&buffer_[0], buffer_.size());
					z_.next_out = reinterpret_cast<byte_type*>(&buffer_[0]);
					z_.avail_out = buffer_.size() * sizeof(char_type);
				}
			}
			return n;
		}
		
		virtual int sync() {
			this->xsputn(NULL, 0);
			return traits::not_eof(traits::eof());
		}
		
	private:
		stream_type& out_;
		container buffer_;
		z_stream z_;
		int flush_;
	};
	
	/* --------------------------------------------------------------------- */
	//  basic_unzstreambuf
	/* --------------------------------------------------------------------- */
	template <
		class CharT = char,
		class Traits = std::char_traits<CharT>
	>
	class basic_unzstreambuf : public std::basic_streambuf<CharT, Traits> {
	public:
		typedef unsigned char byte_type;
		typedef CharT char_type;
		typedef Traits traits;
		typedef std::basic_istream<CharT, Traits> stream_type;
		typedef std::vector<CharT> container;
		typedef typename container::size_type size_type;
		typedef typename Traits::int_type int_type;
		
		basic_unzstreambuf(stream_type& in, size_type n) :
			in_(in), sbuf_(n, 0), dbuf_(n, 0), z_(), flush_(Z_NO_FLUSH) {
			this->setg(&dbuf_[0], &dbuf_[0], &dbuf_[0]);
			z_.zalloc = Z_NULL;
			z_.zfree = Z_NULL;
			z_.opaque = Z_NULL;
			z_.next_in = Z_NULL;
			z_.avail_in = 0;
			inflateInit(&z_);
		}
		
		virtual ~basic_unzstreambuf() throw() { inflateEnd(&z_); }
		
	protected:
		virtual int_type underflow() {
			if (this->egptr() <= this->gptr()) {
				if (flush_ == Z_FINISH) return traits::eof();
				if (!this->xdecompress()) return traits::eof();
				if (this->egptr() <= this->gptr()) return traits::eof();
			}
			return traits::to_int_type(*this->gptr());
		}
		
		virtual int_type uflow() {
			int_type c = this->underflow();
			if (!traits::eq_int_type(c, traits::eof())) this->gbump(1);
			return c;
		}
		
	private:
		stream_type& in_;
		container sbuf_;
		container dbuf_;
		z_stream z_;
		int flush_;
		
		bool xdecompress() {
			z_.next_out = reinterpret_cast<byte_type*>(&dbuf_[0]);
			z_.avail_out = dbuf_.size() * sizeof(char_type);
			
			while (1) {
				if (z_.avail_in == 0) {
					in_.read(&sbuf_[0], sbuf_.size());
					z_.next_in = reinterpret_cast<byte_type*>(&sbuf_[0]);
					z_.avail_in = in_.gcount();
					if (z_.avail_in == 0) flush_ = Z_FINISH;
				}
				
				if (z_.avail_out == 0) {
					this->setg(&dbuf_[0], &dbuf_[0], &dbuf_[0] + dbuf_.size());
					break;
				}
				
				int status = inflate(&z_, flush_);
				if (status == Z_STREAM_END) {
					this->setg(&dbuf_[0], &dbuf_[0], &dbuf_[0] + dbuf_.size() - z_.avail_out / sizeof(char_type));
					break;
				}
				else if (status != Z_OK) return false;
			}
			
			return true;
		}
	};
}

#endif // CLX_ZBUF_H
