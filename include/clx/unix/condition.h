/* ------------------------------------------------------------------------- */
/*
 *  unix/condition.h
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
 *  Distributed under the Boost Software License, Version 1.0. (See
 *  accompanying file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt)
 *
 *  Last-modified: Tue 12 Aug 2008 02:41:23 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_UNIX_CONDITION_H
#define CLX_UNIX_CONDITION_H

#include <pthread.h>
#include <errno.h>
#include "mutex.h"
#include "system_error.h"

namespace clx {
	/* --------------------------------------------------------------------- */
	//  condition
	/* --------------------------------------------------------------------- */
	class condition {
	public:
		typedef pthread_cond_t handle_type;
		typedef handle_type* handle_pointer;
		
		condition() {
			int const res = pthread_cond_init(&cond_, NULL);
			if (res) throw system_error("pthread_cond_init");
		}
		
		virtual ~condition() { pthread_cond_destroy(&cond_); }
		
		template <class LockT>
		void wait(LockT& m) { pthread_cond_wait(&cond_, m.mutex()->native_handle()); }
		
		template <class LockT>
		bool timed_wait(LockT& m, double sec) {
			timeval_type tv;
			tv.tv_sec = static_cast<int>(sec);
			tv.tv_nsec = static_cast<int>((sec - tv.tv_sec) * 1e+09);
			int res = pthread_cond_timedwait(&cond_, m.mutex()->native_handle(), &tv);
			if (res == ETIMEDOUT) return false;
			return true;
		}
		
		void notify_one() { pthread_cond_signal(&cond_); }
		void notify_all() { pthread_cond_broadcast(&cond_); }
		handle_pointer native_handle() { return &cond_; }
		
	private:
		typedef struct timespec timeval_type;
		
		handle_type cond_;
		
		// non-copyable
		condition(const condition& cp);
		condition& operator=(const condition& cp);
	};
}

#endif // CLX_UNIX_CONDITION_H
