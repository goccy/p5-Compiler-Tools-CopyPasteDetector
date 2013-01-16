/* ------------------------------------------------------------------------- */
/*
 *  unix/date_time.h
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
 *  Last-modified: Tue 02 Sep 2008 01:46:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_UNIX_DATE_TIME_H
#define CLX_UNIX_DATE_TIME_H

namespace clx {
	namespace detail {
		/* ----------------------------------------------------------------- */
		//  xlocaltime
		/* ----------------------------------------------------------------- */
		void xlocaltime(const std::time_t* timep, struct tm* result) {
			::localtime_r(timep, result);
		}
		
		/* ----------------------------------------------------------------- */
		//  xgmtime
		/* ----------------------------------------------------------------- */
		void xgmtime(const std::time_t* timep, struct tm* result) {
			::gmtime_r(timep, result);
		}
		
		/* ----------------------------------------------------------------- */
		//  string_to_time
		/* ----------------------------------------------------------------- */
		template <class CharT>
		void string_to_time(const CharT* src, const CharT* fmt, struct tm* dest) {
			::strptime(src, fmt, dest);
		}
		
#ifdef CLX_USE_WCHAR
		template <>
		void string_to_time<wchar_t>(const wchar_t* src, const wchar_t* fmt, struct tm* dest) {
			std::basic_string<char> tmp_src = clx::code_convert<char>(src);
			std::basic_string<char> tmp_fmt = clx::code_convert<char>(fmt);
			::strptime(tmp_src.c_str(), tmp_fmt.c_str(), dest);
		}
#endif // CLX_USE_WCHAR
	}
}

#endif // CLX_UNIX_DATE_TIME_H
