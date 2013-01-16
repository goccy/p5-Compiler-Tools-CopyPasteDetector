/* ------------------------------------------------------------------------- */
/*
 *  unix/filesystem.h
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
#ifndef CLX_UNIX_FILESYSTEM_H
#define CLX_UNIX_FILESYSTEM_H

#include <cstdio>
#include <fstream>
#include <iterator>
#include <string>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

namespace clx {
	namespace filesystem {
		/* ----------------------------------------------------------------- */
		//  delimiter_api
		/* ----------------------------------------------------------------- */
		inline char delimiter_api(const std::basic_string<char>& hint = std::basic_string<char>()) {
			return (hint.find('\\') != std::basic_string<char>::npos) ? '\\' : '/';
		}
		
		/* ----------------------------------------------------------------- */
		//  leaf_api
		/* ----------------------------------------------------------------- */
		inline std::basic_string<char> leaf_api(const std::basic_string<char>& path) {
			const char delim = delimiter_api(path);
			std::basic_string<char>::size_type pos = path.find_last_of(delim);
			if (pos == std::basic_string<char>::npos) return path;
			else if (pos < path.size() - 1) std::basic_string<char>();
			return path.substr(pos + 1);
		}
		
		/* ----------------------------------------------------------------- */
		//  branch_api
		/* ----------------------------------------------------------------- */
		inline std::basic_string<char> branch_api(const std::basic_string<char>& path) {
			const char delim = delimiter_api(path);
			std::basic_string<char>::size_type pos = path.find_last_of(delim);
			if (pos == std::basic_string<char>::npos) return std::basic_string<char>();
			return path.substr(0, pos);
		}
		
		/* ----------------------------------------------------------------- */
		//  exists_api
		/* ----------------------------------------------------------------- */
		inline bool exists_api(const std::basic_string<char>& path) {
			FILE* fp = std::fopen(path.c_str(), "r");
			if (fp) {
				std::fclose(fp);
				return true;
			}
			return false;
		}
		
		/* ----------------------------------------------------------------- */
		//  is_directory_api
		/* ----------------------------------------------------------------- */
		inline bool is_directory_api(const std::basic_string<char>& path) {
			struct stat status;
			if (::stat(path.c_str(), &status) != 0) return false;
			if (S_ISDIR(status.st_mode)) return true;
			return false;
		}
		
		/* ----------------------------------------------------------------- */
		//  copy_api
		/* ----------------------------------------------------------------- */
		inline bool copy_api(const std::basic_string<char>& from, const std::basic_string<char>& to, bool fail_if_exists) {
			if (fail_if_exists && exists_api(to)) return false;
			if (is_directory_api(from)) return false;
			std::basic_ifstream<char> in(from.c_str());
			if (!in) return false;
			
			std::basic_ofstream<char> out(to.c_str());
			std::istreambuf_iterator<char> first(in);
			std::istreambuf_iterator<char> last;
			std::ostreambuf_iterator<char> dest(out);
			std::copy(first, last, dest);
			
			return true;
		}
		
		/* ----------------------------------------------------------------- */
		//  list_api
		/* ----------------------------------------------------------------- */
		template <class OutIter>
		inline OutIter list_api(const std::basic_string<char>& dir, OutIter out) {
			const char delim = (dir.find('\\') != std::basic_string<char>::npos) ? '\\' : '/';
			
			DIR* dp = ::opendir(dir.c_str());
			if (dp == NULL) return out;
			
			struct dirent* entry;
			while ((entry = ::readdir(dp)) != NULL) {
				struct stat status;
				::stat(entry->d_name, &status);
				
				std::basic_string<char> path(entry->d_name);
				if (S_ISDIR(status.st_mode)) path += delim;
				*out = path;
				++out;
			}
			::closedir(dp);
			
			return out;
		}
	}
}

#endif // CLX_UNIX_FILESYSTEM_H
