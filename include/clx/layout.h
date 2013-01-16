/* ------------------------------------------------------------------------- */
/*
 *  layout.h
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
 *  Last-modified: Thu 14 Jan 2010 17:55:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_LAYOUT_H
#define CLX_LAYOUT_H

#include "config.h"
#include <cstdarg>
#include <string>
#include <sstream>

namespace clx {
	/* --------------------------------------------------------------------- */
	//  plain_layout
	/* --------------------------------------------------------------------- */
	class plain_layout {
	public:
		typedef char char_type;
		typedef std::basic_string<char_type> string_type;
		
		plain_layout() : message_() {}
		
		explicit plain_layout(const string_type& s) : message_(s) {}
		
		virtual ~plain_layout() throw() {}
		
		const string_type& operator()(const string_type& s) {
			message_ += s;
			return message_;
		}
		
		const string_type& operator()(const char_type* format, ...) {
			static const int init_size = 1024;
			char_type buf[init_size];
			va_list ap;
			va_start(ap, format);
			int n = vsnprintf(buf, init_size, format, ap);
			va_end(ap);
			
			if (n >= 0 && n < init_size) message_ += buf;
			else if (n >= init_size) {
				char* retry = new char[n + 1];
				va_start(ap, format);
				n = vsnprintf(retry, n + 1, format, ap);
				va_end(ap);
				if (n >= 0) message_ += buf;
				delete [] retry;
			}
			
			return message_;
		}
		
		void prefix(const string_type& s) {
			message_.assign(s);
		}
		
	private:
		string_type message_;
	};
	
	/* --------------------------------------------------------------------- */
	//  trace_layout
	/* --------------------------------------------------------------------- */
	class trace_layout : public plain_layout {
	public:
		trace_layout(const char_type* level, const char_type* date, const char_type* time,
			const char_type* file, int line) :
			plain_layout() {
			std::basic_stringstream<char_type> ss;
			ss << date << " " << time << " ";
			ss << "[" << level << "] ";
			ss << file << ":" << line << ":";
			this->prefix(ss.str());
		}
		
		virtual ~trace_layout() throw() {}
	};
	
	/* --------------------------------------------------------------------- */
	//  date_time_layout
	/* --------------------------------------------------------------------- */
	class date_time_layout : public plain_layout {
	public:
		date_time_layout(const char_type* level, const char_type* date, const char_type* time) :
			plain_layout() {
			std::basic_stringstream<char_type> ss;
			ss << date << " " << time << " ";
			ss << "[" << level << "] ";
			this->prefix(ss.str());
		}
		
		virtual ~date_time_layout() throw() {}
	};
}

#ifndef TRACEF
#define TRACEF clx::trace_layout("TRACE", __DATE__, __TIME__, __FILE__, __LINE__)
#endif // TRACEF

#ifndef DEBUGF
#define DEBUGF clx::trace_layout("DEBUG", __DATE__, __TIME__, __FILE__, __LINE__)
#endif // DEBUGF

#ifndef INFOF
#define INFOF  clx::date_time_layout("INFO ", __DATE__, __TIME__)
#endif // INFOF

#ifndef WARNF
#define WARNF  clx::date_time_layout("WARN ", __DATE__, __TIME__)
#endif // WARNF

#ifndef ERRORF
#define ERRORF clx::date_time_layout("ERROR", __DATE__, __TIME__)
#endif // ERRORF

#ifndef FATALF
#define FATALF clx::date_time_layout("FATAL", __DATE__, __TIME__)
#endif // FATALF

#endif // CLX_LAYOUT_H
