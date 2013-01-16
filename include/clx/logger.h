/* ------------------------------------------------------------------------- */
/*
 *  logger.h
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
#ifndef CLX_LOGGER_H
#define CLX_LOGGER_H

#include "config.h"
#include <ostream>
#include <string>
#include "appender.h"
#include "layout.h"

namespace clx {
	/* --------------------------------------------------------------------- */
	//  loglevel
	/* --------------------------------------------------------------------- */
	namespace loglevel {
		enum { quiet = -1, trace = 10, debug = 20, info = 30, warn = 40, error = 50, fatal = 60 };
	}
	
	/* --------------------------------------------------------------------- */
	//  logger
	/* --------------------------------------------------------------------- */
	class logger {
	public:
		typedef std::size_t size_type;
		typedef char char_type;
		typedef std::basic_string<char> string_type;
		typedef std::basic_ostream<char_type> ostream_type;
		
		static void configure(ostream_type& out, size_type lv = loglevel::error) {
			logger::get_appender() = appender_ptr(new ostream_appender(out));
			logger::level() = lv;
		}
		
		template <class AppenderT>
		static void configure(const AppenderT& cp, size_type lv = loglevel::error) {
			logger::get_appender() = appender_ptr(new AppenderT(cp));
			logger::level() = lv;
		}
		
		static bool put(const string_type& message, size_type lv) {
			bool status = true;
			if (logger::get_appender() && lv >= logger::level()) {
				try {
					status &= logger::get_appender()->reset();
					if (status) {
						ostream_type& out = logger::get_appender()->get(lv);
						if (out) out << message << std::endl;
					}
					status &= logger::get_appender()->finish();
				}
				catch (...) {
					return false;
				}
			}
			return status;
		}
		
		static bool trace(const string_type& message) {
			return logger::is_valid(loglevel::trace) ?
				logger::put(message, loglevel::trace) : true;
		}
		
		static bool debug(const string_type& message) {
			return logger::is_valid(loglevel::debug) ?
				logger::put(message, loglevel::debug) : true;
		}
		
		static bool info(const string_type& message) {
			return logger::is_valid(loglevel::info) ?
				logger::put(message, loglevel::info) : true;
		}
		
		static bool warn(const string_type& message) {
			return logger::is_valid(loglevel::warn) ?
				logger::put(message, loglevel::warn) : true;
		}
		
		static bool error(const string_type& message) {
			return logger::is_valid(loglevel::error) ?
				logger::put(message, loglevel::error) : true;
		}
		
		static bool fatal(const string_type& message) {
			return logger::is_valid(loglevel::fatal) ?
				logger::put(message, loglevel::fatal) : true;
		}
		
	private:
		static appender_ptr& get_appender() {
			static appender_ptr out;
			return out;
		}
		
		static size_type& level() {
			static size_type lv = size_type(loglevel::quiet);
			return lv;
		}
		
		static bool is_valid(size_type lv) {
			return logger::get_appender() && lv >= logger::level();
		}
	};
}

#endif // CLX_LOGGER_H
