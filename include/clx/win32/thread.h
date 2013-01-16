/* ------------------------------------------------------------------------- */
/*
 *  win32/thread.h
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
 *  Last-modified: Sat 25 Oct 2008 05:21:23 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_WIN32_THREAD_H
#define CLX_WIN32_THREAD_H

#include <windows.h>
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
			clx::mutex::scoped_lock lk(dat->data_mutex);
			dat->active = true;
		}
		
		dat->run();
		
		{
			clx::mutex::scoped_lock lk(dat->data_mutex);
			dat->active = false;
		}
		
		return NULL;
	}
	
	/* --------------------------------------------------------------------- */
	//  thread
	/* --------------------------------------------------------------------- */
	class thread {
	public:
		typedef HANDLE handle_pointer;
		typedef DWORD handle_id_type;
		
		thread() : data_(), id_() {}
		
		/*
		 * 2nd param is for the compatibility with the pthread interface.
		 * Despite of the 2nd parameter, win32 thread forces the detached
		 * attribute.
		 */
		template <class Functor>
		explicit thread(Functor f, bool detached = false) :
			data_(new inherit_data<Functor>(f)), id_() {
			data_->joined = detached;
			this->start();
		}
		
		virtual ~thread() throw() {
			if (data_->handle != NULL) {
				if (data_->active) {
					TerminateThread(data_->handle, 0);
					data_->active = false;
				}
				CloseHandle(data_->handle);
			}
		}
		
		template <class Functor>
		void start(Functor f, bool detached = false) {
			data_ = detail::thread_data_ptr(new inherit_data<Functor>(f));
			data_->joined = detached;
			this->start();
		}
		
		bool joinable() const { return !data_->joined; }
		
		void join() {
			if (data_->handle == NULL) return;
			WaitForSingleObject(data_->handle, INFINITE);
		}
		
		void detach() { data_->joined = true; }
		
		void sleep(double sec) {
			clx::timer t;
			DWORD val = static_cast<DWORD>(sec * 1000.0);
			for (int foo = 0; foo < 5; ++foo) {
				mutex mx;
				mutex::scoped_lock lock(mx);
				condition cond;
				cond.timed_wait(lock, val - t.total_elapsed());
				if (t.total_elapsed() > val) return;
			}
		}
		
		handle_id_type get_id() const { return id_; }
		
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
		handle_id_type id_;
		
		// non-copyable
		thread(const thread& cp);
		thread& operator=(const thread& cp);
		
		void start() {
			data_->handle = (handle_pointer)CreateThread(
				NULL, 0, (LPTHREAD_START_ROUTINE)thread_proxy, (void*)data_.get(), 0, &id_);
			if (data_->handle == NULL) throw system_error("CreateThread");
		}
	};
}

#endif // CLX_WIN32_THREAD_H
