/* ------------------------------------------------------------------------- */
/*
 *  vstream.h
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
 *  Last-modified: Wed 02 Jun 2010 05:01:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_VSTREAM_H
#define CLX_VSTREAM_H

#include "config.h"
#include <istream>
#include <streambuf>
#include <string>
#include "range.h"

namespace clx {
	/* --------------------------------------------------------------------- */
	//  basic_ivstreambuf
	/* --------------------------------------------------------------------- */
	template <
		class CharT,
		class Traits = std::char_traits<CharT>
	>
	class basic_ivstreambuf : public std::basic_streambuf<CharT, Traits> {
	public:
		typedef size_t size_type;
		typedef CharT char_type;
		typedef typename Traits::pos_type pos_type;
		typedef typename Traits::off_type off_type;
		
		template <class InIter>
		basic_ivstreambuf(InIter first, InIter last) :
			std::basic_streambuf<CharT, Traits>(),
			begin_(reinterpret_cast<iterator>(&(*first))),
			end_(reinterpret_cast<iterator>(&(*last))) {
			this->setg(begin_, begin_, end_);
		}
		
		template <class Container>
		explicit basic_ivstreambuf(Container& v) :
			std::basic_streambuf<CharT, Traits>(),
			begin_(reinterpret_cast<iterator>(&(*clx::begin(v)))),
			end_(reinterpret_cast<iterator>(&(*clx::end(v)))) {
			this->setg(begin_, begin_, end_);
		}
		
		virtual ~basic_ivstreambuf() throw() {}
		
	protected:
		virtual pos_type seekoff(off_type off, std::ios_base::seekdir way,
			std::ios_base::openmode which = std::ios_base::in) {
			if (which & std::ios_base::in) {
				pos_type last = pos_type(end_ - begin_);
				pos_type pos = 0;
				if (way == std::ios_base::cur) pos = pos_type(this->gptr() - begin_);
				else if (way == std::ios_base::end) pos = last;
				pos += pos_type(off);
				if (pos < 0 || pos > last) return pos_type(-1);
				this->setg(begin_, begin_ + static_cast<int>(pos), end_);
				return pos;
			}
			return pos_type(-1);
		}
		
		virtual pos_type seekpos(pos_type pos, std::ios_base::openmode which = std::ios_base::in) {
			if (which & std::ios_base::in) {
				pos_type last = pos_type(end_ - begin_);
				if (pos < 0 || pos > last) return pos_type(-1);
				this->setg(begin_, begin_ + static_cast<int>(pos), end_);
				return pos;
			}
			return pos_type(-1);
		}
		
		virtual std::streamsize showmanyc() {
			return std::streamsize(-1);
		}
		
	private:
		typedef CharT* iterator;
		
		iterator begin_;
		iterator end_;
	};
	
	/* --------------------------------------------------------------------- */
	//  basic_ivstream
	/* --------------------------------------------------------------------- */
	template <
		class CharT,
		class Traits = std::char_traits<CharT>
	>
	class basic_ivstream : public std::basic_istream<CharT, Traits> {
	public:
		typedef basic_ivstreambuf<CharT, Traits> streambuf_type;
		typedef typename streambuf_type::size_type size_type;
		
		template <class InIter>
		basic_ivstream(InIter first, InIter last) :
			std::basic_istream<CharT, Traits>(0), buf_(first, last) {
			this->rdbuf(&buf_);
		}
		
		template <class Container>
		explicit basic_ivstream(Container& v) :
			std::basic_istream<CharT, Traits>(0), buf_(v) {
			this->rdbuf(&buf_);
		}
		
		virtual ~basic_ivstream() throw() { this->rdbuf(0); }
		
	private:
		streambuf_type buf_;
	};
	
	typedef basic_ivstream<char> ivstream;
}

#endif // CLX_VSTREAM_H
