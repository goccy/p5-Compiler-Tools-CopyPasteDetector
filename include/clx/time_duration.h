/* ------------------------------------------------------------------------- */
/*
 *  time_duration.h
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
 *  Last-modified: Thu 14 Jun 2007 14:38:02 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_TIME_DURATION_H
#define CLX_TIME_DURATION_H

#include "unit.h"

namespace clx {
	/* --------------------------------------------------------------------- */
	//  time_unit
	/* --------------------------------------------------------------------- */
	typedef unit<60,     1> minutes;	// 60 sec
	typedef unit<3600,   1> hours;		// 60 * 60 sec
	typedef unit<86400,  1> days;		// 60 * 60 * 24 sec
	typedef unit<604800, 1> weeks;		// 60 * 60 * 24 * 7 sec
	
	/* --------------------------------------------------------------------- */
	//  date_duration
	/* --------------------------------------------------------------------- */
	class date_duration {
	public:
		typedef int value_type;
		
		explicit date_duration(value_type y = 0, value_type m = 0, value_type d = 0) :
			years_(y), months_(m), days_(d) {}
		
		date_duration& operator+=(const date_duration& t) {
			years_ += t.years_;
			months_ += t.months_;
			days_ += t.days_;
			return *this;
		}
		
		date_duration& operator-=(const date_duration& t) {
			years_ -= t.years_;
			months_ -= t.months_;
			days_ -= t.days_;
			return *this;
		}
		
		// binary operator
		friend date_duration operator+(date_duration x, const date_duration& y) { return x += y; }
		friend date_duration operator-(date_duration x, const date_duration& y) { return x -= y; }
		friend date_duration operator+(const date_duration& y, date_duration x) { return x += y; }
		friend date_duration operator-(const date_duration& y, date_duration x) { return x -= y; }
		
		// accessor
		value_type years() const { return years_; }
		value_type months() const { return months_; }
		value_type days() const { return days_; }
		
	private:
		value_type years_;
		value_type months_;
		value_type days_;
	};
	
	/* --------------------------------------------------------------------- */
	//  helper_functions
	/* --------------------------------------------------------------------- */
	inline date_duration years(int t) { return date_duration(t); }
	inline date_duration months(int t) { return date_duration(0, t); }
}

#endif // CLX_TIME_DURATION_H
