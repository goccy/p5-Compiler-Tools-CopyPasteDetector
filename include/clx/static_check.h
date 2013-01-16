/* ------------------------------------------------------------------------- */
/*
 *  static_check.h
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
 *  This implementation is based on STATIC_CHECK macro described in
 *  A. Alexandrescu, ``Modern C++ Design''.
 *
 *  Last-modified: Fri 30 Jul 2010 21:42:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_STATIC_CHECK_H
#define CLX_STATIC_CHECK_H

#if defined(__BORLANDC__)
#pragma warn -8008
#pragma warn -8066
#endif

template <bool> struct compile_time_checker;
template <> struct compile_time_checker<true> {
	void check_done() {}
};

#define STATIC_CHECK(expr, msg)					\
	if (false) {								\
		compile_time_checker<(expr) != 0>		\
		error_##msg;							\
		(error_##msg).check_done();				\
	}

#endif // CLX_STATIC_CHECK_H
