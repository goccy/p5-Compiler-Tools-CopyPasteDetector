/* ------------------------------------------------------------------------- */
/*
 *  mpl/combine.h
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
 *  Last-modified: Thu 28 Jan 2010 04:22:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_MPL_COMBINE_H
#define CLX_MPL_COMBINE_H

#include <cstddef>
#include <climits>

namespace clx {
	namespace mpl {
		/* ----------------------------------------------------------------- */
		//  combine2
		/* ----------------------------------------------------------------- */
		template <std::size_t N1, unsigned char N2>
		struct combine2 {
			static const std::size_t value = (N1 << CHAR_BIT) | N2;
		};
		
		/* ----------------------------------------------------------------- */
		//  combine3
		/* ----------------------------------------------------------------- */
		template <std::size_t N1, unsigned char N2, unsigned char N3>
		struct combine3 {
			static const std::size_t value = combine2<combine2<N1, N2>::value, N3>::value;
		};
		
		/* ----------------------------------------------------------------- */
		//  combine4
		/* ----------------------------------------------------------------- */
		template <unsigned char N1, unsigned char N2, unsigned char N3, unsigned char N4>
		struct combine4 {
			static const std::size_t value = combine2<combine3<N1, N2, N3>::value, N4>::value;
		};
	} // namespace mpl
} // namespace clx

#endif // CLX_MPL_COMBINE_H
