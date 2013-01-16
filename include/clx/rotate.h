/* ------------------------------------------------------------------------- */
/*
 *  rotate.h
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
 *  Last-modified: Thu 28 Jan 2010 05:28:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_ROTATE_H
#define CLX_ROTATE_H

#include "config.h"
#include <climits>
#include <algorithm>

namespace clx {
	/* --------------------------------------------------------------------- */
	//  rotate_copy
	/* --------------------------------------------------------------------- */
	template <class T>
	inline T rotate_copy(const T& x, std::size_t bits) {
		const std::size_t n = std::min(bits, sizeof(T) * CHAR_BIT);
		return (x << n) | (x >> (sizeof(T) * CHAR_BIT - n));
	}
	
	/* --------------------------------------------------------------------- */
	//  rotate
	/* --------------------------------------------------------------------- */
	template <class T>
	inline T& rotate(T& x, std::size_t bits) {
		x = rotate_copy(x, bits);
		return x;
	}
}

#endif // CLX_ROTATE_H
