/* ------------------------------------------------------------------------- */
/*
 *  timer.h
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
 *  Last-modified: Wed 16 Jul 2008 14:38:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_TIMER_H
#define CLX_TIMER_H

#include "config.h"
#include <stdexcept>

#ifdef CLX_WIN32
#include "win32/timer.h"
#else
#include "unix/timer.h"
#endif

namespace clx {
	/* --------------------------------------------------------------------- */
	//  basic_timer
	/* --------------------------------------------------------------------- */
	template <class GetTimePolicy = gettime_policy>
	class basic_timer {
	public:
		typedef GetTimePolicy policy;
		
		basic_timer() { this->restart(); }
		
		void restart() {
			start_ = policy::gettime();
			point_ = start_;
		}
		
		double update() {
			double tmp = point_;
			point_ = policy::gettime();
			return point_ - tmp;
		}
		
		double total_elapsed() const { return policy::gettime() - start_; }
		double elapsed() const { return policy::gettime() - point_; }
		double precision() const { return policy::precision(); }
		
	private:
		double start_;
		double point_;
	};
	
	typedef basic_timer<> timer;
}

#endif // CLX_TIMER_H
