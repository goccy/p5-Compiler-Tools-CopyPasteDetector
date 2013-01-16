/* ------------------------------------------------------------------------- */
/*
 *  unix/thread.h
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
 *  Last-modified: Sat 25 Oct 2008 05:23:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_UNIX_THREAD_H
#define CLX_UNIX_THREAD_H

#include <pthread.h>
#include "thread_data.h"
#include "mutex.h"
#include "condition.h"
#include "system_error.h"

namespace clx {
	/* --------------------------------------------------------------------- */
	//  thread_proxy
	/* --------------------------------------------------------------------- */
	inline void* thread_proxy(void* param) {
		detail::thread_data* dat = (detail::thread_data*)param;
		
		{
			mutex::scoped_lock lock(dat->data_mutex);
			if (dat->joined) pthread_detach(dat->handle);
		}
		
		dat->run();
		return NULL;
	}
	
	/* --------------------------------------------------------------------- */
	//  thread
	/* --------------------------------------------------------------------- */
	class thread {
	public:
		typedef pthread_t handle_type;
		typedef handle_type* handle_pointer;
		typedef handle_type handle_id_type;
		
		thread() : data_() {}
		
		template <class Functor>
		explicit thread(Functor f, bool detached = false) :
			data_(new inherit_data<Functor>(f)) {
			data_->joined = detached;
			this->start();
		}
		
		virtual ~thread() throw() { this->detach(); }
		
		template <class Functor>
		void start(Functor f, bool detached = false) {
			data_ = detail::thread_data_ptr(new inherit_data<Functor>(f));
			data_->joined = detached;
			this->start();
		}
		
		bool joinable() const { return !data_->joined; }
		
		void join() {
			if (this->joinable()) {
				pthread_join(data_->handle, NULL);
			}
		}
		
		void detach() {
			if (this->joinable()) pthread_detach(data_->handle);
			data_->joined = true;
		}
		
		void sleep(double sec) {
			clx::timer t;
			for (int foo = 0; foo < 5; ++foo) {
				mutex mx;
				mutex::scoped_lock lock(mx);
				condition cond;
				cond.timed_wait(lock, sec - t.total_elapsed());
				if (t.total_elapsed() > sec) return;
			}
		}
		
		handle_id_type get_id() const { return pthread_self(); }
		
	private:
		template <class F>
		class inherit_data : public detail::thread_data {
		public:
			inherit_data(F f) : f_(f) {}
			void run() { f_(); }
		private:
			F f_;
		};
		
		detail::thread_data_ptr data_;
		
		// non-copyable
		thread(const thread& cp);
		thread& operator=(const thread& cp);
		
		void start() {
			int const res = pthread_create(&data_->handle, NULL, thread_proxy, data_.get());
			if (res) throw system_error("pthread_create");
		}
	};
}

#endif // CLX_UNIX_THREAD_H
