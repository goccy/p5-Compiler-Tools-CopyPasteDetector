/* ------------------------------------------------------------------------- */
/*
 *  mutex.h
 *
 *  Copyright (c) 2004 - 2009, clown. All rights reserved.
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
 *  Last-modified: Thu 02 Apr 2009 10:07:23 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_MUTEX_H
#define CLX_MUTEX_H

#include "config.h"

#ifdef CLX_WIN32
#include "win32/mutex.h"
#include "win32/recursive_mutex.h"
#else
#include "unix/mutex.h"
#include "unix/recursive_mutex.h"
#endif

namespace clx {
	/* --------------------------------------------------------------------- */
	//  unique_lock
	/* --------------------------------------------------------------------- */
	template <class Mutex>
	class unique_lock {
	public:
		typedef Mutex mutex_type;
		
		explicit unique_lock(mutex_type& m) : m_(&m) { this->lock(); }
		virtual ~unique_lock() { this->unlock(); }
		
		void lock() { m_->lock(); }
		void unlock() { m_->unlock(); }
		
		mutex_type* mutex() { return m_; }
		const mutex_type* mutex() const { return m_; }
		
	private:
		mutex_type* m_;
	};
}

#endif // CLX_MUTEX_H
