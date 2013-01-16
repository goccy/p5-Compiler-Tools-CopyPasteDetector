/* ------------------------------------------------------------------------- */
/*
 *  filesystem.h
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
 *  Last-modified: Wed 20 Jan 2010 06:16:23 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_FILESYSTEM_H
#define CLX_FILESYSTEM_H

#include "config.h"
#ifdef CLX_WIN32
#include "win32/filesystem.h"
#else
#include "unix/filesystem.h"
#endif

namespace clx {
	namespace filesystem {
		/* ----------------------------------------------------------------- */
		//  delimiter
		/* ----------------------------------------------------------------- */
		inline char delimiter(const std::basic_string<char>& hint = std::basic_string<char>()) {
			return delimiter_api(hint);
		}
		
		/* ----------------------------------------------------------------- */
		//  extension
		/* ----------------------------------------------------------------- */
		inline std::basic_string<char> extension(const std::basic_string<char>& path) {
			std::size_t pos = path.find_last_of('.');
			return (pos != std::basic_string<char>::npos) ? path.substr(pos) : path;
		}
		
		/* ----------------------------------------------------------------- */
		//  basename
		/* ----------------------------------------------------------------- */
		inline std::basic_string<char> basename(const std::basic_string<char>& path) {
			std::size_t pos = path.find_last_of('.');
			return (pos != std::basic_string<char>::npos) ? path.substr(0, pos) : path;
		}
		
		/* ----------------------------------------------------------------- */
		//  leaf
		/* ----------------------------------------------------------------- */
		inline std::basic_string<char> leaf(const std::basic_string<char>& path) {
			return leaf_api(path);
		}
		
		/* ----------------------------------------------------------------- */
		//  brunch
		/* ----------------------------------------------------------------- */
		inline std::basic_string<char> branch(const std::basic_string<char>& path) {
			return branch_api(path);
		}
		
		/* ----------------------------------------------------------------- */
		//  exists
		/* ----------------------------------------------------------------- */
		inline bool exists(const std::basic_string<char>& path) {
			return exists_api(path);
		}
		
		/* ----------------------------------------------------------------- */
		//  is_directory
		/* ----------------------------------------------------------------- */
		inline bool is_directory(const std::basic_string<char>& path) {
			return is_directory_api(path);
		}
		
		/* ----------------------------------------------------------------- */
		//  copy
		/* ----------------------------------------------------------------- */
		inline bool copy(const std::basic_string<char>& from, const std::basic_string<char>& to,
			bool fail_if_exists = false) {
			return copy_api(from, to, fail_if_exists);
		}
		
		/* ----------------------------------------------------------------- */
		//  list
		/* ----------------------------------------------------------------- */
		template <class OutIter>
		inline OutIter list(const std::basic_string<char>& dir, OutIter out) {
			return list_api(dir, out);
		}
	}
}

#endif // CLX_FILESYSTEM_H
