/* ------------------------------------------------------------------------- */
/*
 *  range_iterator.h
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
#ifndef CLX_RANGE_ITERATOR_H
#define CLX_RANGE_ITERATOR_H

#include "config.h"

namespace clx {
	/* --------------------------------------------------------------------- */
	//  range_iterator
	/* --------------------------------------------------------------------- */
	template <class Container>
	struct range_iterator {
		typedef typename Container::iterator type;
	};
	
#if !defined(__BORLANDC__)
	/* --------------------------------------------------------------------- */
	/*
	 *  range_iterator
	 *
	 *  Specialized for const T. I don't know how to realize specialization
	 *  for const T in Borland C++ Compiler.
	 */
	/* --------------------------------------------------------------------- */
	template <class Container>
	struct range_iterator<const Container> {
		typedef typename Container::const_iterator type;
	};
	
	/* --------------------------------------------------------------------- */
	//  range_iterator
	/* --------------------------------------------------------------------- */
	template <class T, std::size_t N>
	struct range_iterator<T[N]> {
		typedef T* type;
	};
	
	/* --------------------------------------------------------------------- */
	//  range_iterator
	/* --------------------------------------------------------------------- */
	template <class T, std::size_t N>
	struct range_iterator<const T[N]> {
		typedef const T* type;
	};
	
	/* --------------------------------------------------------------------- */
	//  begin
	/* --------------------------------------------------------------------- */
	template <class Container>
	inline typename range_iterator<Container>::type begin(Container& c) {
		return c.begin();
	}
	
	/* --------------------------------------------------------------------- */
	//  begin
	/* --------------------------------------------------------------------- */
	template <class T, std::size_t N>
	inline typename range_iterator<T[N]>::type begin(T (&ar)[N]) {
		return ar;
	}
	
	/* --------------------------------------------------------------------- */
	//  end
	/* --------------------------------------------------------------------- */
	template <class Container>
	inline typename range_iterator<Container>::type end(Container& c) {
		return c.end();
	}
	
	/* --------------------------------------------------------------------- */
	//  end
	/* --------------------------------------------------------------------- */
	template <class T, std::size_t N>
	inline typename range_iterator<T[N]>::type end(T (&ar)[N]) {
		return ar + N;
	}
	
#else // for Borland C++ Compiler
	
	/* --------------------------------------------------------------------- */
	//  begin
	/* --------------------------------------------------------------------- */
	template <class Container>
	inline typename Container::iterator begin(Container& c) {
		return c.begin();
	}
	
	/* --------------------------------------------------------------------- */
	//  begin
	/* --------------------------------------------------------------------- */
	template <class Container>
	inline typename Container::const_iterator begin(const Container& c) {
		return c.begin();
	}
	
	/* --------------------------------------------------------------------- */
	//  begin
	/* --------------------------------------------------------------------- */
	template <class T, std::size_t N>
	inline T* begin(T (&ar)[N]) {
		return ar;
	}
	
	/* --------------------------------------------------------------------- */
	//  begin
	/* --------------------------------------------------------------------- */
	template <class T, std::size_t N>
	inline const T* begin(const T (&ar)[N]) {
		return ar;
	}
	
	/* --------------------------------------------------------------------- */
	//  end
	/* --------------------------------------------------------------------- */
	template <class Container>
	inline typename Container::iterator end(Container& c) {
		return c.end();
	}
	
	/* --------------------------------------------------------------------- */
	//  end
	/* --------------------------------------------------------------------- */
	template <class Container>
	inline typename Container::const_iterator end(const Container& c) {
		return c.end();
	}
	
	/* --------------------------------------------------------------------- */
	//  end
	/* --------------------------------------------------------------------- */
	template <class T, std::size_t N>
	inline T* end(T (&ar)[N]) {
		return ar + N;
	}
	
	/* --------------------------------------------------------------------- */
	//  end
	/* --------------------------------------------------------------------- */
	template <class T, std::size_t N>
	inline const T* end(const T (&ar)[N]) {
		return ar + N;
	}
	
#endif // !defined(__BORLANDC__)
}

#endif // CLX_RANGE_ITERATOR_H
