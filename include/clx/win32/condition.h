/* ------------------------------------------------------------------------- */
/*
 *  win32/condition.h
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
 *  Last-modified: Tue 12 Aug 2008 02:41:23 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_WIN32_CONDITION_H
#define CLX_WIN32_CONDITION_H

#include <windows.h>
#include "system_error.h"

namespace clx {
	/* --------------------------------------------------------------------- */
	//  condition
	/* --------------------------------------------------------------------- */
	class condition {
	public:
		typedef HANDLE handle_pointer;
		
		condition() {
			cond_ = CreateEvent(NULL, true, false, NULL);
			if (cond_ == NULL) throw system_error("CreateEvent");
		}
		
		virtual ~condition() { CloseHandle(cond_); }
		
		template <class LockT>
		void wait(LockT& m) {
			m.unlock();
			WaitForSingleObject(cond_, INFINITE);
			m.lock();
		}
		
		template <class LockT>
		bool timed_wait(LockT& m, double sec) {
			DWORD tv = static_cast<DWORD>(sec * 1000.0);
			m.unlock();
			DWORD res = WaitForSingleObject(cond_, tv);
			m.lock();
			if (res == WAIT_TIMEOUT) return false;
			return true;
		}
		
		void notify_one() { SetEvent(cond_); }
		
		// compatibility for the pthread interface
		void notify_all() { SetEvent(cond_); }
		
		handle_pointer native_handle() { return cond_; }
		
	private:
		handle_pointer cond_;
	};
}

#endif // CLX_WIN32_CONDITION_H
