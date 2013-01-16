/* ------------------------------------------------------------------------- */
/*
 *  range_size.h
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
 *  Distributed under the Boost Software License, Version 1.0. (See
 *  accompanying file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt)
 *
 *  Last-modified: Sat 31 Jul 2010 21:07:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_RANGE_SIZE_H
#define CLX_RANGE_SIZE_H

#include "config.h"
#include "range_iterator.h"

namespace clx {
	/* --------------------------------------------------------------------- */
	//  range_size
	/* --------------------------------------------------------------------- */
	template <class Container>
	struct range_size {
		typedef typename Container::size_type type;
	};
	
#if !defined(__BORLANDC__)
	
	/* --------------------------------------------------------------------- */
	//  range_size
	/* --------------------------------------------------------------------- */
	template <class Container>
	struct range_size<const Container> {
		typedef typename Container::size_type type;
	};
	
	/* --------------------------------------------------------------------- */
	//  range_size
	/* --------------------------------------------------------------------- */
	template <class T, std::size_t N>
	struct range_size<T[N]> {
		typedef std::size_t type;
	};
	
	/* --------------------------------------------------------------------- */
	//  range_size
	/* --------------------------------------------------------------------- */
	template <class T, std::size_t N>
	struct range_size<const T[N]> {
		typedef std::size_t type;
	};
	
	/* --------------------------------------------------------------------- */
	//  size
	/* --------------------------------------------------------------------- */
	template <class Container>
	inline typename range_size<Container>::type size(const Container& c) {
		return c.size();
	}
	
	/* --------------------------------------------------------------------- */
	//  size
	/* --------------------------------------------------------------------- */
	template <class T, std::size_t N>
	inline typename range_size<T[N]>::type size(const T (&ar)[N]) {
		return N;
	}
	
#else // for Borland C++ Compiler
	
	/* --------------------------------------------------------------------- */
	//  size
	/* --------------------------------------------------------------------- */
	template <class Container>
	inline typename Container::size_type size(const Container& c) {
		return c.size();
	}
	
	/* --------------------------------------------------------------------- */
	//  size
	/* --------------------------------------------------------------------- */
	template <class T, std::size_t N>
	inline std::size_t size(const T (&ar)[N]) {
		return N;
	}
	
#endif // !defined(__BORLANDC__)
	
	/* --------------------------------------------------------------------- */
	//  empty
	/* --------------------------------------------------------------------- */
	template <class T>
	inline bool empty(const T& r) {
		return begin(r) == end(r);
	}
}

#endif // CLX_RANGE_SIZE_H
