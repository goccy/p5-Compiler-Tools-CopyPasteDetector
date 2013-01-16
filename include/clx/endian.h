/* ------------------------------------------------------------------------- */
/*
 *  endian.h
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
 *  Last-modified: Thu 14 Jan 2010 18:53:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_ENDIAN_H
#define CLX_ENDIAN_H

#include "config.h"

namespace clx {
	namespace endian {
		enum { none = 0, big, little };
		
		/* ----------------------------------------------------------------- */
		//  which
		/* ----------------------------------------------------------------- */
		inline int which() {
			int x = 0x00000001;
			if (*(char*)&x) return little;
			else return big;
		}
		
		/* ----------------------------------------------------------------- */
		//  is_little
		/* ----------------------------------------------------------------- */
		inline bool is_little() {
			return which() == little;
		}
		
		/* ----------------------------------------------------------------- */
		//  is_big
		/* ----------------------------------------------------------------- */
		inline bool is_big() {
			return which() == big;
		}
	}
}

#endif // CLX_ENDIANH
