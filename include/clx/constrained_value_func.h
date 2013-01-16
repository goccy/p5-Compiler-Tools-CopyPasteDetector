/* ------------------------------------------------------------------------- */
/*
 *  constrained_value_func.h
 *
 *  Copyright (c) 2004 - 2006, clown. All rights reserved.
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
 *  The source code is derived from D. Ryan Stephens, Christopher Diggins,
 *  Janathan Turkanis, and Jeff Cogswell, ``C++ Cookbook,'' O'Reilly Media,
 *  Inc, 2006.
 *
 *  Last-modified: Mon 11 Dec 2006 04:02:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_CONSTRAINED_VALUE_FUNC_H
#define CLX_CONSTRAINED_VALUE_FUNC_H

#include "config.h"
#include <stdexcept>

namespace clx {
	template <int Min, int Max>
	class ranged_integer {
	public:
		typedef int value_type;
		const static value_type default_value = Min;
		
		static void assign(value_type& dest, const value_type& src) {
			if ((src < Min) || (src > Max)) {
				throw std::range_error("out of valied range");
			}
			dest = src;
		}
	};
};

#endif // CLX_CONSTRAINED_VALUE_FUNC_H
