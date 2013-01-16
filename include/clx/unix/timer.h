/* ------------------------------------------------------------------------- */
/*
 *  unix/timer.h
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
#ifndef CLX_UNIX_TIMER_H
#define CLX_UNIX_TIMER_H

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

namespace clx {
	/* --------------------------------------------------------------------- */
	//  gettime_policy
	/* --------------------------------------------------------------------- */
	struct use_gettimeofday {
		static double gettime() {
			struct timeval tv;
			::gettimeofday(&tv, NULL);
			
			double sec = static_cast<double>(tv.tv_sec);
			double usec = static_cast<double>(tv.tv_usec) * 1e-6;
			return sec + usec;
		}
		
		static double precision() { return 1.0 * 1e-6; }
	};
	typedef use_gettimeofday gettime_policy;
	
	/* --------------------------------------------------------------------- */
	//  sleep
	/* --------------------------------------------------------------------- */
	inline int sleep(double sec) {
		struct timeval tv;
		tv.tv_sec  = static_cast<int>(sec);
		tv.tv_usec = static_cast<int>(sec * 1000000) % 1000000;
		return ::select(0, NULL, NULL, NULL, &tv);
	}
}

#endif // CLX_UNIX_TIMER_H
