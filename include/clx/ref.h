/* ------------------------------------------------------------------------- */
/*
 *  ref.h
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
 *  Distributed under the Boost Software License, Version 1.0. (See
 *  accompanying file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt)
 *
 *  Last-modified: Wed 25 Nov 2009 22:56:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_REF_H
#define CLX_REF_H

#include "config.h"

namespace clx {
	/* --------------------------------------------------------------------- */
	//  reference_wrapper
	/* --------------------------------------------------------------------- */
	template <class Type>
	class reference_wrapper {
	public:
		typedef Type value_type;
		
		explicit reference_wrapper(value_type& x) :
			x_(&x) {}
		
		operator value_type&() const { return *x_; }
		value_type& get() const { return *x_; }
		value_type* get_pointer() const { return x_; }
		
	private:
		value_type* x_;
	};
	
	/* --------------------------------------------------------------------- */
	//  ref
	/* --------------------------------------------------------------------- */
	template <class T>
	inline reference_wrapper<T> ref(T& x) {
		return reference_wrapper<T>(x);
	}
	
	/* --------------------------------------------------------------------- */
	//  cref
	/* --------------------------------------------------------------------- */
	template <class T>
	inline reference_wrapper<const T> cref(const T& x) {
		return reference_wrapper<const T>(x);
	}
} // namespace clx

#endif // CLX_REF_H
