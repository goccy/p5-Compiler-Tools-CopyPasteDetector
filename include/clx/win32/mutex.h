/* ------------------------------------------------------------------------- */
/*
 *  win32/mutex.h
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
 *  Last-modified: Mon 19 Aug 2008 14:01:23 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_WIN32_MUTEX_H
#define CLX_WIN32_MUTEX_H

#include <windows.h>
#include "system_error.h"

namespace clx {
	template <class Mutex> class unique_lock;
	
	/* --------------------------------------------------------------------- */
	//  basic_mutex
	/* --------------------------------------------------------------------- */
	template <DWORD Timeout>
	class basic_mutex {
	public:
		typedef unique_lock<basic_mutex<Timeout> > scoped_lock;
		typedef HANDLE handle_pointer;
		
		basic_mutex() {
			m_ = CreateMutex(NULL, false, NULL);
			if (m_ == NULL) throw system_error("CreateMutex");
		}
		
		virtual ~basic_mutex() { CloseHandle(m_); }
		
		bool lock() {
			const DWORD res = WaitForSingleObject(m_, Timeout);
			if (res == WAIT_OBJECT_0) return true;
			else return false;
		}
		
		void unlock() { ReleaseMutex(m_); }
		handle_pointer native_handle() { return m_; }
		
	private:
		handle_pointer m_;
		
		// non-copyable
		basic_mutex(const basic_mutex& cp);
		basic_mutex& operator=(const basic_mutex& cp);
	};
	
	typedef basic_mutex<INFINITE> mutex;
	typedef basic_mutex<0> try_mutex;
}

#endif // CLX_WIN32_MUTEX_H
