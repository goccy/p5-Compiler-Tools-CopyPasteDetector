/* ------------------------------------------------------------------------- */
/*
 *  unit.h
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
 *  Last-modified: Tue 22 Sep 2009 16:27:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_UNIT_H
#define CLX_UNIT_H

#include "config.h"

namespace clx {
	/* --------------------------------------------------------------------- */
	//  unit
	/* --------------------------------------------------------------------- */
	template <unsigned int N, unsigned int D = 1>
	class unit {
	public:
		typedef unsigned int uint_t;
		
		static const uint_t num = N;
		static const uint_t den = D;
		
		explicit unit(double x = 0.0) : value_(x) {}
		
		unit& operator=(double x) {
			value_ = x;
			return *this;
		}
		
		unit& operator+=(double x) {
			value_ += x;
			return *this;
		}
		
		unit& operator-=(double x) {
			value_ -= x;
			return *this;
		}
		
		unit& operator*=(double x) {
			value_ *= x;
			return *this;
		}
		
		unit& operator/=(double x) {
			value_ /= x;
			return *this;
		}
		
		operator double () const { return value_ * num / static_cast<double>(den); }
		double value() const { return value_; }
		
	private:
		double value_;
	};
	
	
	/* --------------------------------------------------------------------- */
	//  unit_cast
	/* --------------------------------------------------------------------- */
	template <class Unit>
	inline Unit unit_cast(double x) {
		Unit dest;
		dest = x * Unit::den / static_cast<double>(Unit::num);
		return dest;
	}
	
	/* --------------------------------------------------------------------- */
	/*
	 *  SI unit
	 *
	 *  uint_t cannot represent the following units:
	 *  yocto, zepto, atto, femto, pico, terra, peta, exa, zetta, yotta.
	 */
	/* --------------------------------------------------------------------- */
	typedef unit<1, 1000000000> nano;
	typedef unit<1,    1000000> micro;
	typedef unit<1,       1000> milli;
	typedef unit<1,        100> centi;
	typedef unit<1,         10> deci;
	typedef unit<        10, 1> deca;
	typedef unit<       100, 1> hecto;
	typedef unit<      1000, 1> kilo;
	typedef unit<   1000000, 1> mega;
	typedef unit<1000000000, 1> giga;
	
	// frequently used unit.
	typedef unit<8, 1>   byte;
	typedef unit<1, 100> percent;
	
	/* --------------------------------------------------------------------- */
	//  convert degree <-> radian
	/* --------------------------------------------------------------------- */
	inline double degree(double rad) {
		static const double pi = 3.141592653589793;
		return rad * 180.0 / pi;
	}
	
	inline double radian(double deg) {
		static const double pi = 3.141592653589793;
		return deg * pi / 180.0;
	}
}

#endif // CLX_UNIT_H
