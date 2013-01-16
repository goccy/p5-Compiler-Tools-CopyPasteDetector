/* ------------------------------------------------------------------------- */
/*
 *  unix/mutex.h
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
 *  Last-modified: Mon 19 Aug 2008 14:01:23 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_UNIX_MUTEX_H
#define CLX_UNIX_MUTEX_H

#include <pthread.h>
#include "system_error.h"

namespace clx {
	template <class Mutex> class unique_lock;
	
	/* --------------------------------------------------------------------- */
	//  mutex
	/* --------------------------------------------------------------------- */
	class mutex {
	public:
		typedef unique_lock<mutex> scoped_lock;
		typedef pthread_mutex_t handle_type;
		typedef handle_type* handle_pointer;
		
		mutex() {
			const int res = pthread_mutex_init(&m_, NULL);
			if (res) throw system_error("pthread_mutex_init");
		}
		
		virtual ~mutex() { pthread_mutex_destroy(&m_); }
		
		bool lock() {
			const int res = pthread_mutex_lock(&m_);
			if (!res) return true;
			else throw system_error("pthread_mutex_lock");
		}
		
		void unlock() { pthread_mutex_unlock(&m_); }
		handle_pointer native_handle() { return &m_; }
		
	private:
		handle_type m_;
		
		// non-copyable
		mutex(const mutex& cp);
		mutex& operator=(const mutex& cp);
	};
	
	/* --------------------------------------------------------------------- */
	//  try_mutex
	/* --------------------------------------------------------------------- */
	class try_mutex {
	public:
		typedef unique_lock<try_mutex> scoped_lock;
		typedef pthread_mutex_t handle_type;
		typedef handle_type* handle_pointer;
		
		try_mutex() {
			const int res = pthread_mutex_init(&m_, NULL);
			if (res) throw system_error("pthread_mutex_init");
		}
		
		virtual ~try_mutex() { pthread_mutex_destroy(&m_); }
		
		bool lock() {
			const int res = pthread_mutex_trylock(&m_);
			if (!res) return true;
			else if (res == EBUSY) return false;
			else throw system_error("pthread_mutex_trylock");
		}
		
		void unlock() { pthread_mutex_unlock(&m_); }
		handle_pointer native_handle() { return &m_; }
		
	private:
		handle_type m_;
		
		// non-copyable
		try_mutex(const try_mutex& cp);
		try_mutex& operator=(const try_mutex& cp);
	};
}

#endif // CLX_UNIX_MUTEX_H
