/* ------------------------------------------------------------------------- */
/*
 *  win32/once.h
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
 *  Last-modified: Wed 13 Aug 2008 03:53:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_WIN32_ONCE_H
#define CLX_WIN32_ONCE_H

#include <windows.h>

enum { CLX_NEED_INIT = 0, CLX_DONE_INIT };
#define CLX_ONCE_INIT {CLX_NEED_INIT,0}

namespace clx {
	typedef struct {
		int xo_state;
		long xo_doing;
	} once_flag;
	
	/* --------------------------------------------------------------------- */
	//  call_once
	/* --------------------------------------------------------------------- */
	template <class Functor>
	inline void call_once(once_flag& flag, Functor f) {
		if (flag.xo_state == CLX_NEED_INIT) {
			if (InterlockedIncrement(&flag.xo_doing) == 1) {
				f();
				flag.xo_state = CLX_DONE_INIT;
			}
			else {
				while (flag.xo_state == CLX_NEED_INIT) clx::sleep(0.0);
			}
		}
	}
}

#endif // CLX_WIN32_ONCE_H
