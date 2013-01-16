/* ------------------------------------------------------------------------- */
/*
 *  float_cast.h
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
 *  Last-modified: Thu 28 Jan 2010 15:00:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_FLOAT_CAST_H
#define CLX_FLOAT_CAST_H

#include "config.h"
#include <cmath>
#include <limits>
#include "mpl/bitmask.h"

namespace clx {
	/* --------------------------------------------------------------------- */
	//  fixed_cast
	/* --------------------------------------------------------------------- */
	template <int DecimalBits, typename Type>
	inline double fixed_cast(Type x) {
		double dest = (x >> DecimalBits);
		
		Type mask = 1 << (DecimalBits - 1);
		double digit = 1 / 2.0;
		for (int i = 0; i < DecimalBits; ++i) {
			if (x & mask) dest += digit;
			mask >>= 1;
			digit /= 2.0;
		}
		return dest;
	}
	
	/* --------------------------------------------------------------------- */
	//  float_cast
	/* --------------------------------------------------------------------- */
	template <typename Type>
	inline double float_cast(Type x) {
		if (x == 0) return 0.0;
		
		bool sign = (x & mpl::upper_mask<31>::value) ? true : false;
		Type src = x & mpl::lower_mask<31>::value;
		Type exp = (src & mpl::upper_mask<23>::value) >> 23;
		Type frac = src & mpl::lower_mask<23>::value;
		
		if (src == 0xff) {
			if (frac != 0) return std::numeric_limits<double>::quiet_NaN();
			else if (sign) return -std::numeric_limits<double>::infinity();
			return std::numeric_limits<double>::infinity();
		}
		
		double dest = 1.0 / std::pow(2, static_cast<double>(127 - exp)) * (1 + fixed_cast<23>(frac));
		
		return sign ? -dest : dest;
	}
} // namespace clx

#endif // CLX_FLOAT_CAST_H
