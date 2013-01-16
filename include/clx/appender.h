/* ------------------------------------------------------------------------- */
/*
 *  appender.h
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
#ifndef CLX_APPENDER_H
#define CLX_APPENDER_H

#include "config.h"
#include <ostream>
#include <fstream>
#include <string>
#include "shared_ptr.h"
#include "filesystem.h"
#include "date_time.h"

namespace clx {
	/* --------------------------------------------------------------------- */
	//  appender
	/* --------------------------------------------------------------------- */
	class appender {
	public:
		typedef std::size_t size_type;
		typedef char char_type;
		typedef std::basic_ostream<char_type> ostream_type;
		
		appender() {}
		virtual ~appender() throw() {}
		
		virtual ostream_type& get(size_type level) = 0;
		virtual bool reset() { return true; }
		virtual bool finish() { return true; }
	};
	
	/* --------------------------------------------------------------------- */
	//  ostream_appender
	/* --------------------------------------------------------------------- */
	class ostream_appender : public virtual appender {
	public:
		explicit ostream_appender(ostream_type& out) : appender(), out_(out) {}
		virtual ~ostream_appender() throw() {}
		virtual ostream_type& get(size_type level) { return out_; }
		
	private:
		ostream_type& out_;
		ostream_appender(const ostream_appender& cp);
		ostream_appender& operator=(const ostream_appender& cp);
	};
	
	/* --------------------------------------------------------------------- */
	//  file_appender
	/* --------------------------------------------------------------------- */
	class file_appender : public virtual appender {
	public:
		typedef std::basic_string<char_type> string_type;
		
		explicit file_appender(const string_type& path, int limit = -1, bool left_open = true) :
			appender(), out_(), path_(path), limit_(limit), open_(left_open) {}
		
		virtual ~file_appender() throw() {}
		
		virtual ostream_type& get(size_type level) { return *out_; }
		
		virtual bool reset() {
			if (!out_) out_ = fstream_ptr(new fstream_type(path_.c_str(), std::ios_base::out | std::ios_base::app));
			if (out_->fail()) return false;
			return true;
		}
		
		virtual bool finish() {
			if (out_ && out_->is_open()) {
				long n = out_->tellp();
				if (limit_ > 0 && n >= limit_) {
					out_->close();
					out_ = fstream_ptr();
					
					date_time now;
					string_type to = filesystem::basename(path_)
						+ now.to_string("_%Y%m%d%H%M%S") + filesystem::extension(path_);
					std::rename(path_.c_str(), to.c_str());
				}
				else if (!open_) {
					out_->close();
					out_ = fstream_ptr();
				}
			}
			return true;
		}
		
	private:
		typedef std::basic_ofstream<char_type> fstream_type;
		typedef shared_ptr<fstream_type> fstream_ptr;
		fstream_ptr out_;
		string_type path_;
		long limit_;
		bool open_;
	};
	
	typedef shared_ptr<appender> appender_ptr;
}

#endif // CLX_APPENDER_H
