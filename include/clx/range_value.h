/* ------------------------------------------------------------------------- */
/*
 *  range_value.h
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
#ifndef CLX_RANGE_VALUE_H
#define CLX_RANGE_VALUE_H

#include "config.h"

namespace clx {
	/* --------------------------------------------------------------------- */
	//  range_value
	/* --------------------------------------------------------------------- */
	template <class Container>
	struct range_value {
		typedef typename Container::value_type type;
	};
	
#if !defined(__BORLANDC__)
	/* --------------------------------------------------------------------- */
	//  range_value
	/* --------------------------------------------------------------------- */
	template <class Container>
	struct range_value<const Container> {
		typedef typename Container::value_type type;
	};
	
	/* --------------------------------------------------------------------- */
	//  range_value
	/* --------------------------------------------------------------------- */
	template <class T, std::size_t N>
	struct range_value<T[N]> {
		typedef T type;
	};
	
	/* --------------------------------------------------------------------- */
	//  range_value
	/* --------------------------------------------------------------------- */
	template <class T, std::size_t N>
	struct range_value<const T[N]> {
		typedef T type;
	};
#endif
}

#endif // CLX_RANGE_VALUE_H
