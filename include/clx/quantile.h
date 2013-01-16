/* ------------------------------------------------------------------------- */
/*
 *  quantile.h
 *
 *  Copyright (c) 2004 - 2008, clown. All rights reserved.
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
 *  Last-modified: Tue 23 Jan 2007 00:16:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_QUANTILE_H
#define CLX_QUANTILE_H

#include "config.h"
#include <algorithm>
#include <vector>
#include <cmath>
#include "lexical_cast.h"

namespace clx {
	/* --------------------------------------------------------------------- */
	//  quantile
	/* --------------------------------------------------------------------- */
	template <class InIter>
	inline double quantile(InIter first, InIter last, unsigned int k, unsigned int q) {
		if (k > q) return static_cast<double>(0);
		
		std::vector<double> v;
		while (first != last) v.push_back(lexical_cast<double>(*first++));
		std::sort(v.begin(), v.end());
		
		unsigned int idx = q + k * v.size() - k;
		if (idx % q == 0) return v.at(idx / q - 1);
		
		double t = idx / static_cast<double>(q);
		unsigned int ceil = static_cast<unsigned int>(std::ceil(t));
		unsigned int floor = static_cast<unsigned int>(std::floor(t));
		
		return (ceil - t) * v.at(floor - 1) + (t - floor) * v.at(ceil - 1);
	}
	
	/* --------------------------------------------------------------------- */
	//  median
	/* --------------------------------------------------------------------- */
	template <class InIter>
	inline double median(InIter first, InIter last) {
		return quantile(first, last, 1, 2);
	}
	
	/* --------------------------------------------------------------------- */
	//  tertile
	/* --------------------------------------------------------------------- */
	template <class InIter>
	inline double tertile(InIter first, InIter last, unsigned int k) {
		return quantile(first, last, k, 3);
	}
	
	/* --------------------------------------------------------------------- */
	//  quartile
	/* --------------------------------------------------------------------- */
	template <class InIter>
	inline double quartile(InIter first, InIter last, unsigned int k) {
		return quantile(first, last, k, 4);
	}
	
	/* --------------------------------------------------------------------- */
	//  quintile
	/* --------------------------------------------------------------------- */
	template <class InIter>
	inline double quintile(InIter first, InIter last, unsigned int k) {
		return quantile(first, last, k, 5);
	}
	
	/* --------------------------------------------------------------------- */
	//  decile
	/* --------------------------------------------------------------------- */
	template <class InIter>
	inline double decile(InIter first, InIter last, unsigned int k) {
		return quantile(first, last, k, 10);
	}
	
	/* --------------------------------------------------------------------- */
	//  percentile
	/* --------------------------------------------------------------------- */
	template <class InIter>
	inline double percentile(InIter first, InIter last, unsigned int k) {
		return quantile(first, last, k, 100);
	}
};

#endif // CLX_QUANTILE_H
