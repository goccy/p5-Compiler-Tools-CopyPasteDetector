/* ------------------------------------------------------------------------- */
/*
 *  reverse.h
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
 *  Last-modified: Thu 28 Jan 2010 04:01:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_REVERSE_H
#define CLX_REVERSE_H

#include "config.h"
#include <algorithm>

namespace clx {
	/* --------------------------------------------------------------------- */
	//  reverse
	/* --------------------------------------------------------------------- */
	inline void reverse(unsigned char* src, size_t n) {
		std::reverse(src, src + n);
	}
	
	/* --------------------------------------------------------------------- */
	//  reverse
	/* --------------------------------------------------------------------- */
	template <class Type>
	inline Type& reverse(Type& src) {
		if (sizeof(Type) == 1) return src;
		
		reverse(reinterpret_cast<unsigned char*>(&src), sizeof(Type));
		return src;
	}
	
	/* --------------------------------------------------------------------- */
	//  reverse_copy
	/* --------------------------------------------------------------------- */
	template <class Type>
	inline Type reverse_copy(const Type& src) {
		Type dest = src;
		return reverse(dest);
	}
} // namespace clx

#endif // CLX_REVERSE_H
