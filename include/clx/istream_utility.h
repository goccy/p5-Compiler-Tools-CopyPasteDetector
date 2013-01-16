/* ------------------------------------------------------------------------- */
/*
 *  istream_utility.h
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
 *  Last-modified: Sun 01 Aug 2010 14:42:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_ISTREAM_UTILITY_H
#define CLX_ISTREAM_UTILITY_H

#include "config.h"
#include <istream>
#include <iterator>
#include <string>
#include "endian.h"
#include "memory.h"

namespace clx {
	/* --------------------------------------------------------------------- */
	//  read
	/* --------------------------------------------------------------------- */
	template <class Ch, class Tr, class Type, std::size_t N>
	inline std::size_t read(std::basic_istream<Ch, Tr>& in, Type (&dest)[N], int which = endian::none) {
		typedef Type value_type;
		typedef std::size_t size_type;
		typedef std::basic_istream<Ch, Tr> istream_type;
		typedef std::ios_base ios_type;
		
		size_type extracted = 0;
		typename ios_type::iostate err = ios_type::goodbit;
		typename std::basic_istream<Ch, Tr>::sentry se(in, true);
		if (se) {
			typename istream_type::int_type c = in.rdbuf()->sgetc();
			while (extracted < N && !Tr::eq_int_type(c, Tr::eof())) {
				value_type elem;
				if (in.rdbuf()->sgetn(reinterpret_cast<Ch*>(&elem), sizeof(value_type)) <= 0) {
					err |= ios_type::failbit;
					break;
				}
				if (which != endian::none && which != endian::which()) clx::reverse(elem);
				dest[extracted] = elem;
				++extracted;
				c = in.rdbuf()->sgetc();
			}
			if (Tr::eq_int_type(c, Tr::eof())) err |= ios_type::eofbit;
		}
		
		if (!extracted) err |= ios_type::failbit;
		if (err) in.setstate(err);
		
		return extracted;
	}
	
	/* --------------------------------------------------------------------- */
	//  read
	/* --------------------------------------------------------------------- */
	template <class Ch, class Tr, class Container>
	inline std::basic_istream<Ch, Tr>& read(std::basic_istream<Ch, Tr>& in, Container& dest, int which = endian::none) {
		typedef typename Container::value_type value_type;
		typedef typename Container::size_type size_type;
		typedef std::basic_istream<Ch, Tr> istream_type;
		typedef std::ios_base ios_type;
		
		size_type extracted = 0;
		const size_type n = dest.max_size();
		typename ios_type::iostate err = ios_type::goodbit;
		typename std::basic_istream<Ch, Tr>::sentry se(in, true);
		if (se) {
			try {
				dest.clear();
				std::insert_iterator<Container> out(dest, dest.end());
				
				typename istream_type::int_type c = in.rdbuf()->sgetc();
				while (extracted < n && !Tr::eq_int_type(c, Tr::eof())) {
					value_type elem;
					if (in.rdbuf()->sgetn(reinterpret_cast<Ch*>(&elem), sizeof(value_type)) <= 0) {
						err |= ios_type::failbit;
						break;
					}
					if (which != endian::none && which != endian::which()) clx::reverse(elem);
					*out = elem;
					++out;
					++extracted;
					c = in.rdbuf()->sgetc();
				}
				if (Tr::eq_int_type(c, Tr::eof())) err |= ios_type::eofbit;
				else err |= ios_type::failbit; // larger than max_size
			}
			catch (...) {
				in.setstate(std::ios_base::badbit);
			}
		}
		
		if (!extracted) err |= ios_type::failbit;
		if (err) in.setstate(err);
		
		return in;
	}
	
	/* --------------------------------------------------------------------- */
	//  get
	/* --------------------------------------------------------------------- */
	template <class Ch, class Tr, class Type>
	inline std::basic_istream<Ch, Tr>& get(std::basic_istream<Ch, Tr>& in, Type& dest, int which = endian::none) {
		typedef std::size_t size_type;
		typedef std::basic_istream<Ch, Tr> istream_type;
		typedef std::ios_base ios_type;
		
		typename ios_type::iostate err = ios_type::goodbit;
		typename std::basic_istream<Ch, Tr>::sentry se(in, true);
		if (se) {
			typename istream_type::int_type c = in.rdbuf()->sgetc();
			if (Tr::eq_int_type(c, Tr::eof())) err |= (ios_type::eofbit | ios_type::failbit);
			else if (in.rdbuf()->sgetn(reinterpret_cast<Ch*>(&dest), sizeof(Type)) <= 0) {
				err |= ios_type::failbit;
			}
			else if (which != endian::none && which != endian::which()) clx::reverse(dest);
		}
		
		if (err) in.setstate(err);
		
		return in;
	}
}

#endif // CLX_ISTREAM_UTILITY_H
