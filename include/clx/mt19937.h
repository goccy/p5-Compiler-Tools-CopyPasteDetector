/* ------------------------------------------------------------------------- */
/*
 *  mt19937.h
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
 *    - The names of its contributors may not be used to endorse or promote
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
 *  Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
 *  All rights reserved.
 *
 *  The source code is derived from the Mersenne Twister algorithm proposed
 *  by Takuji Nishimura and Makoto Matsumoto. (See the following URL:
 *  http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/emt.html)
 *
 *  Last-modified: Thu 09 Oct 2008 06:44:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_MT19937_H
#define CLX_MT19937_H

#include "config.h"
#include <vector>

namespace clx {
	/* --------------------------------------------------------------------- */
	//  mt19937
	/* --------------------------------------------------------------------- */
	class mt19937 {
	public:
		typedef unsigned int uint_type;
		typedef unsigned long int ulong_type;
		
		static const uint_type ntbl = 624;
		static const uint_type mval = 397;
		static const ulong_type matrix_a = 0x9908b0dfUL; // constant vector a
		static const ulong_type umask = 0x80000000UL; // most significant w-r bits
		static const ulong_type lmask = 0x7fffffffUL; // least significant r bits
		
		explicit mt19937(ulong_type s = 5489UL) : word_(), index_(ntbl + 1) {
			word_.reserve(ntbl);
			this->seed(s);
		}
		
		/* ----------------------------------------------------------------- */
		//  initializes word_ with a seed
		/* ----------------------------------------------------------------- */
		void seed(ulong_type s) {
			word_.clear();
			word_.push_back(s & 0xffffffffUL);
			for (index_ = 1; index_ < ntbl; index_++) {
				word_.push_back(1812433253UL * (word_[index_-1] ^ (word_[index_-1] >> 30)) + index_); 
				/*
				 * See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier.
				 * In the previous versions, MSBs of the seed affect
				 * only MSBs of the array word_.
				 * 2002/01/09 modified by Makoto Matsumoto
				 * 2008/10/09 modified the description mt[] -> word_ by clown.
				 */
				word_[index_] &= 0xffffffffUL; // for >32 bit machines
			}
		}
		
		/* ----------------------------------------------------------------- */
		/*
		 *  initialize by an array with array-length
		 *  init_key is the array for initializing keys
		 *  key_length is its length
		 *  slight change for C++, 2004/2/26
		 */
		/* ----------------------------------------------------------------- */
		template <class SeedArray>
		void seed(SeedArray& gen) {
			this->seed(19650218UL);
			
			uint_type i = 1;
			uint_type j = 0;
			uint_type k = (ntbl > gen.size() ? ntbl : gen.size());
			
			for (; k; --k) {
				word_[i] = (word_[i] ^ ((word_[i-1] ^ (word_[i-1] >> 30)) * 1664525UL)) + gen[j] + j; // non linear
				word_[i] &= 0xffffffffUL; // for WORDSIZE > 32 machines
				++i;
				++j;
				if (i >= ntbl) {
					word_[0] = word_[ntbl-1];
					i=1;
				}
				if (j >= gen.size()) j = 0;
			}
			
			for (k = ntbl - 1; k; --k) {
				word_[i] = (word_[i] ^ ((word_[i-1] ^ (word_[i-1] >> 30)) * 1566083941UL)) - i; // non linear
				word_[i] &= 0xffffffffUL; // for WORDSIZE > 32 machines
				++i;
				if (i >= ntbl) {
					word_[0] = word_[ntbl-1];
					i = 1;
				}
			}
			
			word_[0] = 0x80000000UL; // MSB is 1; assuring non-zero initial array
		}
		
		void operator()(ulong_type& dest) {
			static unsigned long mag01[2] = {0x0UL, matrix_a}; // mag01[x] = x * MATRIX_A  for x=0,1
			
			if (index_ >= ntbl) { // generate N words at one time
				uint_type kk;
				
				if (index_ == ntbl + 1) this->seed(5489UL); // a default initial seed is used
				
				for (kk = 0; kk < ntbl - mval; ++kk) {
					dest = (word_[kk] & umask) | (word_[kk+1] & lmask);
					word_[kk] = word_[kk + mval] ^ (dest >> 1) ^ mag01[dest & 0x1UL];
				}
				
				for (; kk < ntbl - 1; ++kk) {
					dest = (word_[kk] & umask) | (word_[kk+1] & lmask);
					word_[kk] = word_[kk + (mval - ntbl)] ^ (dest >> 1) ^ mag01[dest & 0x1UL];
				}
				
				dest = (word_[ntbl-1] & umask) | (word_[0] & lmask);
				word_[ntbl-1] = word_[mval-1] ^ (dest >> 1) ^ mag01[dest & 0x1UL];
				
				index_ = 0;
			}
			
			dest = word_[index_++];
			
			// Tempering
			dest ^= (dest >> 11);
			dest ^= (dest << 7) & 0x9d2c5680UL;
			dest ^= (dest << 15) & 0xefc60000UL;
			dest ^= (dest >> 18);
		}
		
		template <typename ValueT>
		void operator()(ValueT& dest) {
			ulong_type tmp;
			(*this)(tmp);
			dest = static_cast<ValueT>(tmp >> (sizeof(ulong_type) - sizeof(ValueT)));
		}
		
		/* ----------------------------------------------------------------- */
		/*
		 *  There are four methods for generating a real random value
		 *  in the original source code.
		 *
		 *  genrand_real1: return genrand_int32()*(1.0/4294967295.0);
		 *  genrand_real2: return genrand_int32()*(1.0/4294967296.0);
		 *  genrand_real3: return (((double)genrand_int32()) + 0.5)
		 *    * (1.0/4294967296.0);
		 *  genrand_real53:
		 *    unsigned long a=genrand_int32()>>5, b=genrand_int32()>>6;
		 *    return (a*67108864.0+b)*(1.0/9007199254740992.0);
		 *
		 *  We choise the genrand_real3 method.
		 */
		/* ----------------------------------------------------------------- */
		void operator()(double& dest) {
			ulong_type tmp;
			(*this)(tmp);
			dest = (static_cast<double>(tmp) + 0.5) * (1.0 / 4294967296.0);
		}
		
	private:
		std::vector<ulong_type> word_; // the array for the state vector
		uint_type index_; // (index_ == ntbl + 1) means word_[ntbl] is not initialized
	};
}

#endif // CLX_MT19937_H
