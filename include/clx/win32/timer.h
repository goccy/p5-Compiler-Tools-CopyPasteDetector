/* ------------------------------------------------------------------------- */
/*
 *  timer.h
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
 *  Last-modified: Fri 30 Jul 2010 19:48:03 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_WIN32_TIMER_H
#define CLX_WIN32_TIMER_H

#include "../config.h"
#include <windows.h>

#ifdef CLX_ENABLE_AUTOLINK
#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma comment(lib, "winmm.lib")
#endif
#endif // CLX_ENABLE_AUTOLINK

namespace clx {
	namespace detail {
		/* ----------------------------------------------------------------- */
		/*
		 *  high_clock_frequency
		 *
		 *  The high_clock_frequency class (and the static variable of
		 *  the class) changes the clock resolution of Windows.
		 *  Although the default value of clock resolution is 15ms, it
		 *  is too coarse in some cases. So we change the value to 1ms
		 *  while running programs using this library.
		 */
		/* ----------------------------------------------------------------- */
		class high_clock_frequency {
		public:
			high_clock_frequency() { ::timeBeginPeriod(1); }
			~high_clock_frequency() throw() { ::timeEndPeriod(1); }
		};
		static high_clock_frequency clkfreq_;
	}
	
	/* --------------------------------------------------------------------- */
	//  gettime_policy
	/* --------------------------------------------------------------------- */
	struct use_time_get_time {
		static double gettime() {
			return static_cast<double>(::timeGetTime()) * 1e-3;
		}
		
		static double precision() { return 1.0 * 1e-3; }
	};
	
	struct use_query_performance {
		static double gettime() {
			LARGE_INTEGER freq, now;
			std::memset(&freq, 0, sizeof(freq));
			std::memset(&now, 0, sizeof(now));
			
			if (!::QueryPerformanceFrequency(&freq)) {
				return static_cast<double>(::timeGetTime()) * 1e-3;
			}
			::QueryPerformanceCounter(&now);
			return now.QuadPart / static_cast<double>(freq.QuadPart);
		}
		
		static double precision() {
			LARGE_INTEGER freq;
			std::memset(&freq, 0, sizeof(freq));
			if (!::QueryPerformanceFrequency(&freq)) {
				return 1.0 * 1e-3;
			}
			return 1.0 / static_cast<double>(freq.QuadPart);
		}
	};
	
#ifdef CLX_USE_HIGH_PRECISION_TIMER
	typedef use_query_performance gettime_policy;
#else
	typedef use_time_get_time gettime_policy;
#endif
	
	/* --------------------------------------------------------------------- */
	//  sleep
	/* --------------------------------------------------------------------- */
	inline int sleep(double sec) {
		::Sleep(static_cast<int>(sec * 1000));
		return 0;
	}
}

#endif // CLX_WIN32_TIMER_H
